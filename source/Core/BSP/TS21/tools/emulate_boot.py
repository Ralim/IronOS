"""Smoke-test the TS21 ELF's startup and task switching, not physical hardware.

Peripheral values and interrupt delivery are simulated. Instruction counts are
an approximate timebase, not cycle-accurate timing or a watchdog qualification.
"""

import argparse
from collections import deque
from pathlib import Path
import struct
import subprocess
import sys

from inspect_image import inspect
from unicorn import Uc, UcError, UC_ARCH_RISCV, UC_MODE_RISCV32
from unicorn import UC_HOOK_CODE, UC_HOOK_MEM_READ, UC_HOOK_MEM_WRITE
from unicorn.riscv_const import UC_RISCV_REG_PC, UC_RISCV_REG_X0

parser = argparse.ArgumentParser()
parser.add_argument("elf", type=Path)
parser.add_argument("--prefix", required=True)
parser.add_argument("--inherited-interrupt", action="store_true")
parser.add_argument("--instructions", type=int, default=40000000)
parser.add_argument("--max-frame-us", type=int)
parser.add_argument("--check-cold-display", action="store_true")
parser.add_argument("--fahrenheit", action="store_true")
parser.add_argument("--check-worklight", choices=("off", "on", "menu"))
parser.add_argument(
    "--check-heater",
    choices=(
        "startup",
        "short",
        "open",
        "probe",
        "supply",
        "overtemp",
        "stall",
        "continuous",
        "range",
        "uart",
        "monitor",
        "recover",
    ),
)
args = parser.parse_args()
TRANSIENT_CHECKS = ("open", "probe", "supply", "overtemp", "continuous", "range")
if args.check_cold_display and args.check_heater:
    parser.error("Use separate runs for cold-display and heater checks")
if args.check_worklight and (args.check_cold_display or args.check_heater):
    parser.error("Use a separate run for work-light checks")

symbols = {}
symbol_sizes = {}
for line in subprocess.check_output(
    [args.prefix + "nm", "-S", "-n", "-C", str(args.elf)], text=True
).splitlines():
    fields = line.split(maxsplit=3)
    if len(fields) == 4:
        symbols[fields[3]] = int(fields[0], 16)
        symbol_sizes[fields[3]] = int(fields[1], 16)
    elif len(fields) == 3:
        symbols[fields[2]] = int(fields[0], 16)

image = args.elf.read_bytes()
inspect(image)
header = struct.unpack_from("<16sHHIIIIIHHHHHH", image)
cpu = Uc(UC_ARCH_RISCV, UC_MODE_RISCV32)
for base, length in [
    (0x08000000, 0x10000),
    (0x20000000, 0x5000),
    (0x40000000, 0x24000),
    (0xE000E000, 0x2000),
]:
    cpu.mem_map(base, length)
cpu.mem_write(0x08000000, b"\xff" * 0x10000)
for i in range(header[10]):
    kind, offset, virtual, physical, size, memsize, flags, align = struct.unpack_from(
        "<IIIIIIII", image, header[5] + i * header[9]
    )
    if kind == 1 and size:
        cpu.mem_write(physical, image[offset : offset + size])


class Board:
    def __init__(self):
        self.steps = 0
        self.csr = {}
        self.registers = {}
        self.counter = 0
        self.counter_origin = 0
        self.enabled = 0
        self.pending = 0
        self.active_irq = args.inherited_interrupt
        self.last_compare = None
        self.gpio = {0x40010800: 0, 0x40010C00: 0}
        self.i2c_active = False
        self.i2c_pulses = 0
        self.i2c_byte = 0
        self.i2c_packet = []
        self.display_bytes = 0
        self.frame_start = None
        self.frame_bytes = 0
        self.frame_pixels = bytearray()
        self.latest_frame = None
        self.frames_us = []
        self.display_on = False
        self.idle_check_started = False
        self.idle_draws = 0
        self.interrupts = {12: 0, 14: 0}
        self.last_watchdog = 0
        self.result = None
        self.heater_commands = 0
        self.first_heater_us = None
        self.modulation_seen = False
        self.expected_target = 454 if args.fahrenheit else 320
        self.target_tip_adc = 2992 if args.fahrenheit else 2180
        self.range_started = None
        self.transient_started = None
        self.transient_restored = False
        self.gate_started = None
        self.home_draws = 0
        self.button_started = None
        self.button_mask = 1
        self.button_duration_us = 120000
        self.injected = False
        self.fault_draws = 0
        self.fault_stage = "waiting"
        self.expect_recovery = args.check_heater in (
            "monitor",
            "recover",
        )
        self.recovery_idle_draws = 0
        self.recovery_heater_commands = 0
        self.disconnect_return = None
        self.last_heater_mode = None
        self.stall_requested = False
        self.tip_adc = 580
        self.probe_delta = 354
        self.input_adc = 2900
        self.sample_adc = {}
        self.worklight_stage = "boot"
        self.worklight_boot_frames = 0
        self.worklight_off_pixels = None
        self.original_settings = None
        self.expected_fonts = {}
        if "font12_out_buffer" in symbols:
            sys.path.insert(
                0, str(Path(__file__).resolve().parents[5] / "Translations")
            )
            import brieflz

            for buffer, source in (
                ("font12_out_buffer", "font_12x16_brieflz"),
                ("font06_out_buffer", "font_06x08_brieflz"),
                ("translation_data_out_buffer", "translation_data_brieflz"),
            ):
                if buffer not in symbols:
                    continue
                self.expected_fonts[buffer] = brieflz.depack_srcsize(
                    bytes(cpu.mem_read(symbols[source], symbol_sizes[source])),
                    symbol_sizes[buffer],
                )
        if args.check_heater and "heater_fault_samples" not in symbols:
            raise ValueError("--check-heater requires the heater-test firmware")
        self.recent = deque(maxlen=24)
        self.trace = {}
        for name, address in symbols.items():
            if name in (
                "main",
                "startGUITask",
                "startPIDTask",
                "startMOVTask",
                "startPOWTask",
                "xPortStartFirstTask",
                "ts21_run_constructors",
            ) or name.startswith("flash_save_buffer"):
                self.trace[address] = name
        self.seen = set()
        self.required_tasks = {
            symbols[name]
            for name in ("startGUITask", "startPIDTask", "startMOVTask", "startPOWTask")
        }
        self.instructions = {}
        text = subprocess.check_output(
            [args.prefix + "objdump", "-d", str(args.elf)], text=True
        )
        for line in text.splitlines():
            fields = line.strip().split()
            if (
                len(fields) >= 3
                and fields[0].endswith(":")
                and len(fields[1]) in (4, 8)
            ):
                try:
                    address, instruction = int(fields[0][:-1], 16), int(fields[1], 16)
                except ValueError:
                    continue
                if instruction & 0x7F == 0x73:
                    self.instructions[address] = instruction

    def now(self):
        if self.registers.get(0xE000F000, 0) & 1:
            return self.counter + (self.steps - self.counter_origin) // 8
        return self.counter

    def core_hz(self):
        clock = self.registers.get(0x40021004, 0)
        if clock & 12 == 8:
            multiplier = (clock >> 18 & 15) + 2
            if multiplier == 17:
                multiplier = 18
            source = 8000000 if self.registers.get(0x40023800, 0) & 0x10 else 4000000
            return source * multiplier
        return 8000000

    def micros(self):
        return self.now() * 8000000 // self.core_hz()

    def stop(self, reason):
        self.result = reason
        cpu.emu_stop()

    def seed_cold_display_settings(self):
        count = symbol_sizes["settingsConstants"] // 8
        constants = cpu.mem_read(symbols["settingsConstants"], count * 8)
        defaults = [
            struct.unpack_from("<4H", constants, i * 8)[3] for i in range(count)
        ]
        defaults[7] = 7  # Nonzero saved motion sensitivity, despite no motion driver.
        defaults[10] = 1  # Enter the real soldering UI through normal autostart.
        if args.check_heater:
            defaults[0] = 850 if args.fahrenheit else 320
            defaults[15] = int(args.fahrenheit)
            defaults[18] = 20  # Saved power-bank keepalive must not bypass arming.
            defaults[22] = 176  # Saved boost must not bypass the test limit.
            defaults[24] = (
                5  # The prior 5 W setting must not hide the requested 100 W ceiling.
            )
            defaults[57] = 1
        if args.check_worklight:
            defaults[0] = 350
            defaults[10] = 0
            defaults[57] = int(args.check_worklight == "on")
            if args.check_worklight != "on":
                count = 57  # Exercise migration from the previous firmware.
                defaults = defaults[:count]
        record = struct.pack("<HH" + "H" * count, 0x55AA, count, *defaults)
        cpu.mem_write(0x0800FC00, record.ljust(256, b"\xff"))

    def ram_value(self, name, size=4):
        return int.from_bytes(cpu.mem_read(symbols[name], size), "little")

    def status_errors(self):
        return int.from_bytes(cpu.mem_read(symbols["ts21_status"] + 4, 4), "little")

    def worklight_value(self):
        return int.from_bytes(
            cpu.mem_read(symbols["systemSettings"] + 4 + 57 * 2, 2), "little"
        )

    def worklight_press(self, mask):
        self.button_mask = mask
        self.button_started = self.micros()

    def worklight_checkbox(self):
        return b"".join(
            self.latest_frame[page * 128 + 114 : page * 128 + 126] for page in (1, 2)
        )

    def seek_worklight_menu(self):
        # Select the real menu descriptor directly; subsequent toggles/exit use GPIO buttons.
        context_size = symbol_sizes["context"]
        if context_size not in (28, 32):
            raise ValueError("Unexpected guiContext ABI")
        menu = bytes(cpu.mem_read(symbols["UIMenu"], symbol_sizes["UIMenu"]))
        entries = []
        for stride in (20, 28):
            if len(menu) % stride == 0:
                entries.extend(
                    offset // stride
                    for offset in range(0, len(menu), stride)
                    if menu[offset + 16] == 57
                )
        submenus = bytes(
            cpu.mem_read(symbols["subSettingsMenus"], symbol_sizes["subSettingsMenus"])
        )
        roots = [
            offset // 4
            for offset in range(0, len(submenus), 4)
            if struct.unpack_from("<I", submenus, offset)[0] == symbols["UIMenu"]
        ]
        if len(entries) != 1 or len(roots) != 1:
            raise ValueError(
                "The work-light setting is not present exactly once in User interface"
            )
        context = bytearray(context_size)
        struct.pack_into("<I", context, 0, self.ram_value("xTickCount"))
        struct.pack_into("<HH", context, context_size - 20, roots[0], entries[0] + 1)
        cpu.mem_write(symbols["context"], bytes(context))
        cpu.mem_write(
            symbols["currentOperatingMode"],
            (4).to_bytes(symbol_sizes["currentOperatingMode"], "little"),
        )

    def check_worklight_gui(self):
        mode = self.ram_value("currentOperatingMode", 1)
        if mode != self.last_heater_mode:
            print(
                f"{self.micros():8d} us work-light mode {mode}, stage={self.worklight_stage}",
                flush=True,
            )
            self.last_heater_mode = mode
        light = bool(self.gpio[0x40010800] & (1 << 9))
        setting = self.worklight_value()
        if (
            self.status_errors()
            or self.registers.get(0x40000400, 0) & 1
            or self.gate_high()
        ):
            self.stop("FAIL: work-light interaction changed heater/fault state")
            return
        if self.worklight_stage == "boot" and mode == 0:
            self.worklight_boot_frames += 1
            if self.worklight_boot_frames < 3:
                return
            expected = int(args.check_worklight == "on")
            if (
                setting != expected
                or light != bool(expected)
                or (self.registers.get(0x40010804, 0) >> 4) & 15 != 3
            ):
                self.stop(
                    "FAIL: work-light default/restored state or PA9 configuration is incorrect"
                )
                return
            if args.check_worklight != "menu":
                self.stop(
                    f"PASS: work light restored {'on' if expected else 'off'}, heater remains off"
                )
                return
            self.original_settings = bytes(
                cpu.mem_read(symbols["systemSettings"] + 4, 57 * 2)
            )
            self.seek_worklight_menu()
            self.worklight_stage = "ready"
        elif self.worklight_stage == "ready":
            self.worklight_off_pixels = self.worklight_checkbox()
            self.worklight_press(1)
            self.worklight_stage = "turn-on"
        elif self.worklight_stage == "turn-on" and setting == 1 and light:
            self.worklight_stage = "on-visible"
        elif self.worklight_stage == "on-visible":
            if self.worklight_checkbox() == self.worklight_off_pixels:
                self.stop("FAIL: work-light checkbox did not change to on")
                return
            self.worklight_press(1)
            self.worklight_stage = "turn-off"
        elif self.worklight_stage == "turn-off" and setting == 0 and not light:
            self.worklight_stage = "off-visible"
        elif self.worklight_stage == "off-visible":
            if self.worklight_checkbox() != self.worklight_off_pixels:
                self.stop("FAIL: work-light checkbox did not return to off")
                return
            self.worklight_press(1)
            self.worklight_stage = "enable-and-save"
        elif self.worklight_stage == "enable-and-save" and setting == 1 and light:
            cpu.mem_write(
                symbols["context"] + symbol_sizes["context"] - 18, b"\x00\x00"
            )
            self.worklight_press(3)
            self.worklight_stage = "saving"
        elif self.worklight_stage == "saving" and mode == 0:
            saved = bytes(cpu.mem_read(0x0800FC00, 124))
            if (
                struct.unpack_from("<HH", saved) != (0x55AA, 58)
                or struct.unpack_from("<H", saved, 118)[0] != 1
                or saved[4:118] != self.original_settings
                or not light
            ):
                self.stop(
                    "FAIL: work-light menu exit did not preserve and save the selected setting"
                )
                return
            self.stop(
                "PASS: work-light checkbox toggles PA9 on/off, saves on, preserves prior settings and never heats"
            )

    def update_gate(self):
        if self.gate_started is None or not self.registers.get(0x40012C00, 0) & 1:
            return
        elapsed = self.micros() - self.gate_started
        count = elapsed * self.core_hz() // ((self.registers[0x40012C28] + 1) * 1000000)
        if count > self.registers[0x40012C2C]:
            if self.registers[0x40012C00] & 8:
                self.registers[0x40012C00] &= ~1
                count = 0
        self.registers[0x40012C24] = count

    def gate_high(self):
        self.update_gate()
        return (
            self.registers.get(0x40010804, 0) >> 8 & 15 == 0xB
            and self.registers.get(0x40012C44, 0) & 0x8000
            and self.registers.get(0x40012C20, 0) & 0x100
            and self.registers.get(0x40012C24, 0) >= self.registers.get(0x40012C3C, 1)
        )

    def check_heater_command(self):
        if not args.check_heater or args.check_heater == "startup":
            self.stop("FAIL: heater timer enabled without an armed heater test")
            return
        required = {
            0x40000400: 0x81,
            0x40000418: 0x68,
            0x40000420: 1,
            0x40000428: 0,
            0x4000042C: 639,
            0x40012C00: 0x89,
            0x40012C1C: 0x78,
            0x40012C20: 0x100,
            0x40012C28: 63999,
            0x40012C2C: 120,
            0x40012C3C: 1,
            0x40012C44: 0x8000,
        }
        if any(
            self.registers.get(address) != value for address, value in required.items()
        ):
            self.stop("FAIL: heater PWM or one-pulse gate configuration is incorrect")
            return
        if (
            not self.gpio[0x40010800] & (1 << 9)
            or (self.registers.get(0x40010804, 0) >> 4) & 15 != 3
        ):
            self.stop("FAIL: heater control changed the enabled work light")
            return
        compare = self.registers[0x40000434]
        voltage = (self.sample_adc["input"] * 69 - 88) // 1000 + 1
        resistance = (self.sample_adc["probe"] - self.sample_adc["tip"]) * 10 // 177
        if (
            not 0 < compare <= 640 * 85 // 100
            or compare * voltage * voltage > 1000 * resistance * 640
        ):
            self.stop(f"FAIL: first-heating power/duty limit exceeded: CCR1={compare}")
            return
        maximum = min(640 * 1000 * resistance // (voltage * voltage), 640 * 85 // 100)
        if (
            (
                args.check_heater not in ("continuous", "range")
                or self.heater_commands < 3
            )
            and self.fault_stage != "recovering"
            and not self.transient_restored
            and compare != maximum
        ):
            self.stop(
                f"FAIL: cold fixture did not reach the requested 100 W ceiling: CCR1={compare}"
            )
            return
        reported = self.ram_value("heater_power_x10", symbol_sizes["heater_power_x10"])
        expected_power = compare * (voltage * voltage // resistance) // 640
        if reported != expected_power:
            self.stop(
                f"FAIL: accepted-power readback truncated: {reported} != {expected_power}"
            )
            return
        self.modulation_seen |= compare < maximum
        if (
            self.button_started is None
            or self.ram_value("currentOperatingMode", 1) != 1
            or self.ram_value("currentTempTargetDegC") != self.expected_target
        ):
            self.stop("FAIL: heating was not manually started with a bounded target")
            return
        if (
            self.injected
            and self.fault_stage != "recovering"
            and not self.transient_restored
        ):
            self.stop("FAIL: heater rearmed after an injected fault")
            return
        if args.check_heater == "uart" and not self.status_errors() & (1 << 4):
            self.stop("FAIL: passive UART receive error was not retained for diagnosis")
            return
        if self.first_heater_us is None:
            self.first_heater_us = self.micros()
        self.heater_commands += 1
        print(
            f"{self.micros():8d} us heater command {self.heater_commands}: "
            f"CCR1={compare}/640, target={self.ram_value('currentTempTargetDegC')} C",
            flush=True,
        )
        if self.fault_stage == "recovering":
            if (
                self.status_errors()
                or not self.ram_value("heaterArmed", 1)
                or self.ram_value("heaterThermalRunawayCounter", 1)
            ):
                self.stop("FAIL: a fresh manual start did not restore bounded heating")
                return
            self.recovery_heater_commands += 1
            if self.recovery_heater_commands < 3:
                return
            if compare != maximum:
                self.stop(
                    "FAIL: recovered cold heating did not reach the power ceiling"
                )
                return
            self.stop(
                "PASS: healthy fault acknowledged without reboot or automatic heating; "
                "a fresh manual press/release restarted within the power limit"
            )
            return
        if self.transient_restored:
            if self.status_errors() or self.ram_value("heaterThermalRunawayCounter", 1):
                self.stop("FAIL: a self-correcting reading left a latched fault")
                return
            self.stop(
                "PASS: the self-correcting reading withheld power without stopping the "
                "iron, and heating resumed on its own once the reading recovered"
            )
            return
        if self.heater_commands == 3 and args.check_heater == "range":
            self.range_started = self.micros()
            self.tip_adc = self.target_tip_adc
        elif self.heater_commands == 3 and args.check_heater != "continuous":
            self.injected = True
            if args.check_heater in ("short", "uart", "recover"):
                # A measurable element below 1.0 ohm. A zero delta is an unreadable probe phase,
                # not a short, and is covered by "probe".
                self.probe_delta = 20
            elif args.check_heater == "open":
                self.probe_delta = 4095 - self.tip_adc
            elif args.check_heater == "probe":
                self.probe_delta = 0
            elif args.check_heater == "supply":
                self.input_adc = 0
            elif args.check_heater == "overtemp":
                self.tip_adc = (
                    3328  # Exactly 500 C with the fixture handle temperature.
                )
            elif args.check_heater == "monitor":
                cpu.mem_write(symbols["heaterThermalRunawayCounter"], b"\xff")
            elif args.check_heater == "stall":
                self.stall_requested = True
                cpu.emu_stop()
            if args.check_heater in TRANSIENT_CHECKS:
                self.transient_started = self.micros()

    def restore_fixture(self):
        # Always return to a cold tip: restoring to the target would leave the controller with
        # nothing to ask for, so resumed heating could never be observed.
        self.probe_delta = 354
        self.input_adc = 2900
        self.tip_adc = 580

    def check_stalled_gate(self):
        steps = self.steps
        self.counter += 1000 * 8
        if not self.gate_high():
            self.result = "FAIL: one-pulse gate did not become active"
            return
        self.counter += 120001 * 8
        if self.gate_high() or self.registers[0x40012C00] & 1 or self.steps != steps:
            self.result = "FAIL: stalled CPU left the heater gate on"
        else:
            self.result = "PASS: hardware gate expired with no further CPU instructions"

    def check_fault_pixels(self, error):
        expected = bytearray(256)
        font = symbols["font06_out_buffer"]
        for i, digit in enumerate(f"{error:08X}"):
            glyph = cpu.mem_read(font + int(digit, 16) * 16, 16)
            for page in range(2):
                offset = page * 128 + 32 + i * 8
                expected[offset : offset + 8] = glyph[page * 8 : page * 8 + 8]
        if self.latest_frame is None or self.latest_frame[256:] != expected:
            self.stop(f"FAIL: stop code {error:08X} is not fully visible on the OLED")
            return False
        return True

    def check_heater_gui(self):
        mode = self.ram_value("currentOperatingMode", 1)
        if mode != self.last_heater_mode:
            print(f"{self.micros():8d} us GUI mode {mode}", flush=True)
            self.last_heater_mode = mode
        if (
            self.fault_stage == "acknowledging"
            and self.micros() - self.button_started < self.button_duration_us
            and mode != 9
        ):
            self.stop("FAIL: acknowledgment left the warning while its button was held")
            return
        if self.fault_stage == "recovering-home":
            if (
                mode != 0
                or self.status_errors()
                or self.ram_value("heaterArmed", 1)
                or self.ram_value("currentTempTargetDegC")
                or self.ram_value("heaterThermalRunawayCounter", 1)
                or self.gate_high()
                or self.registers.get(0x40000400, 0) & 1
            ):
                self.stop(
                    "FAIL: acknowledgment did not leave a healthy, unarmed home screen"
                )
                return
            self.recovery_idle_draws += 1
            if self.recovery_idle_draws >= 3:
                self.fault_stage = "recovering"
                self.button_mask = 1
                self.button_duration_us = 120000
                self.button_started = self.micros()
                print(
                    "Starting again with a new bottom-button press/release", flush=True
                )
        if (
            args.check_heater == "continuous"
            and self.first_heater_us is not None
            and not self.injected
            and self.micros() - self.first_heater_us >= 6000000
        ):
            if (
                mode != 1
                or self.status_errors()
                or not self.modulation_seen
                or self.ram_value("currentTempTargetDegC") != self.expected_target
                or not self.ram_value("heaterArmed", 1)
                or self.gate_high()
                or self.registers.get(0x40000400, 0) & 1
            ):
                self.stop(
                    "FAIL: controller did not regulate continuously beyond the old short limits"
                )
                return
            self.injected = True
            self.tip_adc = 3328
            self.transient_started = self.micros()
            print(
                "Continuous normal-range control held the target; checking the 500 C cutoff",
                flush=True,
            )
        if (
            args.check_heater == "range"
            and self.range_started is not None
            and not self.injected
            and self.micros() - self.range_started >= 300000
        ):
            if (
                mode != 1
                or self.status_errors()
                or self.ram_value("currentTempTargetDegC") != self.expected_target
                or self.sample_adc["tip"] != self.target_tip_adc
                or not self.ram_value("heaterArmed", 1)
            ):
                self.stop("FAIL: saved normal-range setpoint was not held at target")
                return
            self.injected = True
            self.tip_adc = 3328
            self.transient_started = self.micros()
            print(
                "Saved temperature retained above 100 C with no false fault; checking the 500 C cutoff",
                flush=True,
            )
        if (
            self.transient_started is not None
            and not self.transient_restored
            and args.check_heater in TRANSIENT_CHECKS
        ):
            if self.status_errors() or mode == 9:
                self.stop(
                    "FAIL: a self-correcting reading stopped the iron instead of "
                    "withholding power"
                )
                return
            # The 120 ms one-pulse gate is deliberately not revocable, so let any pulse that was
            # already in flight expire before requiring the output to stay off. Starting a new
            # pulse is caught separately by the rearm check in record_heater_command.
            if self.micros() - self.transient_started >= 150000 and (
                self.gate_high() or self.registers.get(0x40000400, 0) & 1
            ):
                self.stop("FAIL: the out-of-range reading did not withhold power")
                return
            if self.micros() - self.transient_started >= 1500000:
                self.transient_restored = True
                self.restore_fixture()
                print(
                    "Power stayed off with no stop warning; checking that heating "
                    "resumes once the reading recovers",
                    flush=True,
                )
        if self.injected and mode == 0:
            if self.fault_stage == "warning":
                self.stop("FAIL: stop warning auto-dismissed without acknowledgment")
                return
            if self.fault_stage == "acknowledging":
                self.fault_stage = "home"
        if mode == 1 and not self.idle_check_started:
            inactive_since = struct.pack(
                "<I", (self.ram_value("xTickCount") - 61000) & 0xFFFFFFFF
            )
            for name in ("lastMovementTime", "lastButtonTime"):
                cpu.mem_write(symbols[name], inactive_since)
            self.idle_check_started = True
        if mode == 0 and not self.heater_commands:
            self.home_draws += 1
            if args.check_heater == "uart" and self.home_draws == 1:
                self.registers[0x40004400] = 8
                print(
                    "Injecting a passive UART receive overrun with a valid tip",
                    flush=True,
                )
            if self.home_draws == 3:
                if args.check_heater == "startup":
                    self.stop(
                        "PASS: saved autostart/boost/keepalive cannot start heating"
                    )
                    return
                self.button_started = self.micros()
                print("Pressing/releasing the physical bottom button", flush=True)
            if (
                args.check_heater == "uart"
                and self.button_started is not None
                and self.micros() - self.button_started > 600000
            ):
                self.stop(
                    "FAIL: passive UART error marks a valid tip disconnected and blocks soldering"
                )
                return
        if self.injected and mode == 9:
            if not self.gpio[0x40010800] & (1 << 9):
                self.stop("FAIL: heater shutdown changed the saved work-light state")
                return
            errors = {
                "short": 1 << 9,
                "uart": 1 << 9,
                "recover": 1 << 9,
                "monitor": 1 << 16,
            }
            error = self.status_errors()
            if (
                error & errors.get(args.check_heater, 0)
                and not self.gate_high()
                and not self.registers.get(0x40000400, 0) & 1
                and not self.ram_value("heaterArmed", 1)
            ):
                self.fault_draws += 1
                if self.fault_stage == "waiting":
                    self.fault_stage = "warning"
                if self.fault_stage == "warning" and self.fault_draws >= 4:
                    if not self.check_fault_pixels(error & ~(1 << 4)):
                        return
                    if args.check_heater == "recover":
                        self.restore_fixture()
                    self.fault_stage = "acknowledging"
                    self.button_mask = 1 if args.check_heater == "recover" else 2
                    self.button_duration_us = (
                        600000 if args.check_heater == "recover" else 120000
                    )
                    self.button_started = self.micros()
                    print("Acknowledging the persistent stop warning", flush=True)
                elif self.fault_stage == "retrying" and self.fault_draws >= 2:
                    fault = (
                        "short after a passive UART receive error"
                        if args.check_heater == "uart"
                        else args.check_heater
                    )
                    self.stop(
                        f"PASS: {fault} fault latched, heater off, "
                        "stop code visible until acknowledged, tip state accurate, "
                        "restart refused"
                    )

    def code(self, uc, pc, size, unused):
        self.steps += 1
        self.recent.append(pc)
        if args.check_heater and self.fault_stage == "home":
            if pc == symbols.get("isTipDisconnected"):
                self.disconnect_return = uc.reg_read(UC_RISCV_REG_X0 + 1)
            elif pc == self.disconnect_return:
                disconnected = bool(uc.reg_read(UC_RISCV_REG_X0 + 10))
                expected = args.check_heater in ("short", "open", "uart")
                if disconnected != expected:
                    self.stop(
                        "FAIL: latched heater stop is confused with tip connection"
                    )
                    return
                if self.expect_recovery:
                    self.fault_stage = "recovering-home"
                    self.button_started = None
                else:
                    self.fault_stage = "retrying"
                    self.fault_draws = 0
                    self.button_mask = 1
                    self.button_started = self.micros()
                    print(
                        "Checking that a bottom-button retry cannot restart heating",
                        flush=True,
                    )
        if pc in self.trace and pc not in self.seen:
            if (
                args.check_cold_display or args.check_heater or args.check_worklight
            ) and pc == symbols["main"]:
                self.seed_cold_display_settings()
            self.seen.add(pc)
            print(
                f"{self.micros():8d} us {self.trace[pc]} sp={uc.reg_read(UC_RISCV_REG_X0 + 2):08x}",
                flush=True,
            )
        if pc == symbols.get("ts21_fault"):
            self.stop(
                f"FAULT error={uc.reg_read(UC_RISCV_REG_X0 + 10):x} "
                f"caller={uc.reg_read(UC_RISCV_REG_X0 + 1):08x}"
            )
            return
        instruction = self.instructions.get(pc)
        if instruction == 0x30200073:
            status = self.csr.get(0x300, 0)
            self.csr[0x300] = (status & ~0x1888) | ((status & 0x80) >> 4) | 0x80
            self.active_irq = False
            uc.reg_write(UC_RISCV_REG_PC, self.csr[0x341])
            return
        if instruction is not None and instruction >> 12 & 7:
            number, operation = instruction >> 20, instruction >> 12 & 7
            source, destination = instruction >> 15 & 31, instruction >> 7 & 31
            value = (
                source
                if operation & 4
                else uc.reg_read(UC_RISCV_REG_X0 + source) if source else 0
            )
            previous = self.csr.get(number, 0)
            mode = operation & 3
            updated = (
                value
                if mode == 1
                else previous | value if mode == 2 else previous & ~value
            )
            self.csr[number] = updated & 0xFFFFFFFF
            if destination:
                uc.reg_write(UC_RISCV_REG_X0 + destination, previous)
            uc.reg_write(UC_RISCV_REG_PC, pc + 4)
            return
        if self.registers.get(0xE000F000, 0) & 2:
            compare = (
                self.registers.get(0xE000F010, 0)
                | self.registers.get(0xE000F014, 0) << 32
            )
            if self.now() >= compare and self.last_compare != compare:
                self.last_compare = compare
                self.pending |= 1 << 12
        if not self.active_irq and self.csr.get(0x300, 0) & 8:
            ready = self.pending & self.enabled
            for irq in (12, 14):
                if ready & (1 << irq):
                    self.pending &= ~(1 << irq)
                    self.interrupts[irq] += 1
                    self.active_irq = True
                    status = self.csr[0x300]
                    self.csr[0x300] = (status & ~0x88) | ((status & 8) << 4) | 0x1800
                    self.csr[0x341] = pc
                    self.csr[0x342] = 0x80000000 | irq
                    vector = (self.csr[0x305] & ~3) + irq * 4
                    uc.reg_write(
                        UC_RISCV_REG_PC,
                        int.from_bytes(uc.mem_read(vector, 4), "little"),
                    )
                    return
        if (
            len(self.frames_us) >= 2
            and self.interrupts[12] >= 100
            and self.interrupts[14]
            and self.required_tasks <= self.seen
        ):
            if self.expected_fonts:
                for buffer, expected in self.expected_fonts.items():
                    if bytes(uc.mem_read(symbols[buffer], len(expected))) != expected:
                        self.stop(f"FAIL: {buffer} did not decompress losslessly")
                        return
                self.expected_fonts.clear()
            if (
                args.max_frame_us is not None
                and max(self.frames_us) > args.max_frame_us
            ):
                self.stop(f"FAIL: simulated frame exceeds {args.max_frame_us} us")
            elif args.check_worklight:
                if (
                    self.worklight_stage == "boot"
                    and self.button_started is None
                    and self.ram_value("currentOperatingMode", 1) == 12
                ):
                    self.worklight_press(2)
                if pc == symbols["guiHandleDraw()"]:
                    self.check_worklight_gui()
            elif args.check_heater:
                if pc == symbols["guiHandleDraw()"]:
                    self.check_heater_gui()
            elif not args.check_cold_display:
                self.stop(
                    "PASS: all four tasks, two full display frames and 100 timer interrupts"
                )
            elif pc == symbols["guiHandleDraw()"]:
                mode = int.from_bytes(
                    uc.mem_read(symbols["currentOperatingMode"], 1), "little"
                )
                if mode != 1:
                    return
                if not self.idle_check_started:
                    tick = int.from_bytes(
                        uc.mem_read(symbols["xTickCount"], 4), "little"
                    )
                    # Age only the activity timestamps, without skipping scheduler ticks.
                    inactive_since = struct.pack("<I", (tick - 61000) & 0xFFFFFFFF)
                    for name in ("lastMovementTime", "lastButtonTime"):
                        uc.mem_write(symbols[name], inactive_since)
                    self.idle_check_started = True
                    print(
                        "Checking cold soldering UI after 61 seconds without activity",
                        flush=True,
                    )
                else:
                    self.idle_draws += 1
                    if self.idle_draws >= 3:
                        self.stop(
                            "PASS: cold soldering display stays on after inactivity"
                            if self.display_on
                            else "FAIL: cold soldering display is off after inactivity"
                        )

    def read(self, uc, access, address, size, value, unused):
        if address == 0x40012C00:
            self.update_gate()
        result = self.registers.get(address, 0)
        if address == 0x40021000:
            result = (result & ~(1 << 25)) | ((result & (1 << 24)) << 1) | 2
        elif address == 0x40021004:
            result = (result & ~12) | ((result & 3) << 2)
            self.registers[address] = result
        elif address == 0x40010808:
            result = (1 << 5) | (1 << 7)
            if (
                self.button_started is not None
                and self.micros() - self.button_started < self.button_duration_us
            ):
                if self.button_mask & 1:
                    result &= ~(1 << 7)
                if self.button_mask & 2:
                    result &= ~(1 << 5)
        elif address == 0x40010C08:
            result = self.gpio[0x40010C00]
            if self.i2c_active and self.i2c_pulses and self.i2c_pulses % 9 == 0:
                result &= ~(1 << 11)
        elif address == 0x40012408:
            if result & 4:
                self.registers[0x4001244C] = 1024
            result &= ~12
            self.registers[address] = result
        elif address == 0x40004404:
            self.registers[0x40004400] = 0
        elif address == 0xE000F008:
            result = self.now() & 0xFFFFFFFF
        elif address == 0xE000F00C:
            result = self.now() >> 32
        uc.mem_write(address, int(result).to_bytes(size, "little"))

    def write(self, uc, access, address, size, value, unused):
        self.registers[address] = value
        if address == 0x4002100C and value & (1 << 11):
            for register in range(0x40012C00, 0x40012C50, 4):
                self.registers[register] = 0
        elif address == 0x40021010 and value & 2:
            for register in range(0x40000400, 0x40000450, 4):
                self.registers[register] = 0
        elif address == 0x40012C00 and value & 1:
            self.gate_started = self.micros()
            self.check_heater_command()
        elif address == 0x40000400 and value & 1 and not args.check_heater:
            self.stop("FAIL: heater carrier enabled in a no-heating run")
        if address in (0x40010810, 0x40010814, 0x40010C10, 0x40010C14):
            base = address & ~0xFF
            old = self.gpio[base]
            new = old | value if address & 0xFF == 0x10 else old & ~value
            self.gpio[base] = new
            if base == 0x40010800 and new & ((1 << 6) | (1 << 10)):
                self.stop("FAIL: heater pin driven high")
            if base == 0x40010C00:
                scl, sda = 1 << 10, 1 << 11
                if old & scl and new & scl:
                    if old & sda and not new & sda:
                        self.i2c_active = True
                        self.i2c_pulses = self.i2c_byte = 0
                        self.i2c_packet = []
                    elif not old & sda and new & sda:
                        self.i2c_active = False
                        if self.i2c_packet in ([0x78, 0x00, 0xAE], [0x78, 0x00, 0xAF]):
                            self.display_on = self.i2c_packet[2] == 0xAF
                            if self.idle_check_started and not self.display_on:
                                self.stop(
                                    "FAIL: cold soldering display blanked after inactivity"
                                )
                        if self.i2c_packet == [0x78, 0x00, 0xB0]:
                            self.frame_start = self.micros()
                            self.frame_bytes = 0
                            self.frame_pixels.clear()
                        if self.i2c_packet[:2] == [0x78, 0x40]:
                            count = len(self.i2c_packet) - 2
                            self.display_bytes += count
                            self.frame_bytes += count
                            self.frame_pixels.extend(self.i2c_packet[2:])
                            if self.frame_start is not None and self.frame_bytes == 512:
                                self.frames_us.append(self.micros() - self.frame_start)
                                self.latest_frame = bytes(self.frame_pixels)
                                self.frame_start = None
                if self.i2c_active and not old & scl and new & scl:
                    self.i2c_pulses += 1
                    if self.i2c_pulses % 9:
                        self.i2c_byte = (self.i2c_byte << 1) | bool(new & sda)
                    else:
                        self.i2c_packet.append(self.i2c_byte)
                        self.i2c_byte = 0
        elif address == 0x40012408 and value & 0x400000:
            if self.gate_high() or self.registers.get(0x40000400, 0) & 1:
                self.stop("FAIL: ADC sampled while the heater was active")
                return
            channel = self.registers.get(0x40012434)
            self.registers[0x40012400] = 2
            if (
                args.check_heater == "continuous"
                and self.first_heater_us is not None
                and not self.injected
            ):
                elapsed = self.micros() - self.first_heater_us
                delta = self.target_tip_adc - 580
                self.tip_adc = 580 + min(delta, delta * elapsed // 2000000)
            tip = self.tip_adc + (self.probe_delta if self.gpio[0x40010800] & 1 else 0)
            self.registers[0x4001244C] = {
                1: tip,
                2: 2864,
                4: 100,
                9: self.input_adc,
            }[channel]
            if channel == 1:
                self.sample_adc["probe" if self.gpio[0x40010800] & 1 else "tip"] = tip
            elif channel == 9:
                self.sample_adc["input"] = self.input_adc
        elif address == 0x4002200C:
            self.registers[address] = 0
        elif address == 0x40022010 and value & 0x20040 == 0x20040:
            target = self.registers[0x40022014]
            if target != 0x0800FC00:
                self.stop(f"FAIL: unexpected flash erase at {target:08x}")
            uc.mem_write(target, b"\xff" * 256)
        elif address == 0xE000E100:
            self.enabled |= value
        elif address == 0xE000E180:
            self.enabled &= ~value
        elif address == 0xE000E200:
            self.pending |= value
        elif address == 0xE000E280:
            self.pending &= ~value
        elif address in (0xE000F008, 0xE000F00C):
            self.counter = (
                self.registers.get(0xE000F008, 0)
                | self.registers.get(0xE000F00C, 0) << 32
            )
            self.counter_origin = self.steps
        elif address == 0x40003000:
            self.last_watchdog = self.micros()
        elif address == 0xE000E048 and value == 0xBEEF0080:
            self.stop("FAIL: firmware requested reset")


board = Board()
cpu.hook_add(UC_HOOK_CODE, board.code)
for start, end in [(0x40000000, 0x40023FFF), (0xE000E000, 0xE000FFFF)]:
    cpu.hook_add(UC_HOOK_MEM_READ, board.read, begin=start, end=end)
    cpu.hook_add(UC_HOOK_MEM_WRITE, board.write, begin=start, end=end)
try:
    cpu.emu_start(header[4], 0, count=args.instructions)
except UcError as error:
    board.result = f"EMULATION ERROR: {error}, PC={cpu.reg_read(UC_RISCV_REG_PC):08x}"
if board.stall_requested:
    board.check_stalled_gate()
print(board.result or "FAIL: instruction budget exhausted", flush=True)
print(
    f"steps={board.steps}, us={board.micros()}, core_hz={board.core_hz()}, "
    f"IRQ={board.interrupts}, display_bytes={board.display_bytes}, frame_us={board.frames_us}, "
    f"watchdog_gap_us={board.micros() - board.last_watchdog}, CSRs={board.csr}",
    flush=True,
)
print("recent PCs: " + " ".join(f"{pc:08x}" for pc in board.recent), flush=True)
if args.check_heater:
    errors = board.status_errors()
    print(
        f"heater_commands={board.heater_commands}, errors={errors:#x}, "
        f"mode={board.ram_value('currentOperatingMode', 1)}, "
        f"armed={board.ram_value('heaterArmed', 1)}, "
        f"warning_draws={board.fault_draws}, gate_high={bool(board.gate_high())}",
        flush=True,
    )
if args.check_worklight:
    print(
        f"worklight_stage={board.worklight_stage}, mode={board.ram_value('currentOperatingMode', 1)}, setting={board.worklight_value()}, PA9={bool(board.gpio[0x40010800] & (1 << 9))}",
        flush=True,
    )
raise SystemExit(0 if board.result and board.result.startswith("PASS") else 1)
