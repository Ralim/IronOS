import struct
import unittest

from tools import generate_font, inspect_image


def jal(displacement):
    return (
        ((displacement >> 20) & 1) << 31
        | ((displacement >> 1) & 0x3FF) << 21
        | ((displacement >> 11) & 1) << 20
        | ((displacement >> 12) & 0xFF) << 12
        | 0x6F
    )


def image():
    start = inspect_image.APP_START
    payload = bytearray(272)
    struct.pack_into("<I", payload, 0, jal(260))
    struct.pack_into("<I", payload, 4, start)
    for vector in range(1, 64):
        struct.pack_into("<I", payload, 4 + vector * 4, start + 264)
    struct.pack_into("<II", payload, 260, 0x13, 0x13)
    header = inspect_image.ELF_HEADER.pack(
        b"\x7fELF\x01\x01\x01" + bytes(9),
        2,
        243,
        1,
        start,
        52,
        0,
        1,
        52,
        32,
        2,
        0,
        0,
        0,
    )
    flash = inspect_image.PROGRAM_HEADER.pack(
        1, 256, start, start, len(payload), len(payload), 5, 4
    )
    ram = inspect_image.PROGRAM_HEADER.pack(
        1, 0, inspect_image.RAM_START, inspect_image.RAM_START, 0, 1024, 6, 4
    )
    data = bytearray(header + flash + ram)
    data.extend(bytes(256 - len(data)))
    data.extend(payload)
    return data


class ImageTests(unittest.TestCase):
    def test_valid_layout(self):
        report = inspect_image.inspect(image())
        self.assertEqual(report["entry"], "0x08007000")
        self.assertEqual(report["vector_base"], "0x08007004")
        self.assertEqual(report["flash_loaded_bytes"], 272)
        self.assertEqual(report["ram_static_bytes"], 1024)
        self.assertEqual(report["status"], "layout-valid; hardware-unvalidated")

    def reject_u32(self, offset, value):
        data = image()
        struct.pack_into("<I", data, offset, value)
        with self.assertRaises(ValueError):
            inspect_image.inspect(data)

    def test_bootloader_entry_rejected(self):
        self.reject_u32(24, 0x08000000)

    def test_bootloader_load_rejected(self):
        self.reject_u32(52 + 12, 0x08006000)

    def test_settings_load_rejected(self):
        self.reject_u32(52 + 12, 0x0800FC00)

    def test_ram_stack_overlap_rejected(self):
        self.reject_u32(84 + 20, 0x5000)

    def test_outside_ram_rejected(self):
        self.reject_u32(84 + 8, 0x20010000)

    def test_truncated_file_rejected(self):
        for size in (0, 51, 60, 115, 270, 527):
            with self.subTest(size=size), self.assertRaises(ValueError):
                inspect_image.inspect(image()[:size])

    def test_invalid_machine_rejected(self):
        data = image()
        struct.pack_into("<H", data, 18, 40)
        with self.assertRaises(ValueError):
            inspect_image.inspect(data)

    def test_float_abi_rejected(self):
        self.reject_u32(36, 5)

    def test_bad_reset_instruction_rejected(self):
        self.reject_u32(256, 0x13)

    def test_reset_jump_to_bootloader_rejected(self):
        self.reject_u32(256, jal(-4))

    def test_bad_reset_vector_rejected(self):
        self.reject_u32(260, 0x08000000)

    def test_bad_interrupt_vector_rejected(self):
        self.reject_u32(264, 0x08006A30)

    def test_unmapped_interrupt_vector_rejected(self):
        self.reject_u32(264, 0x0800E000)

    def test_filesz_larger_than_memsz_rejected(self):
        self.reject_u32(52 + 20, 1)

    def test_invalid_alignment_rejected(self):
        self.reject_u32(52 + 28, 3)

    def test_initialized_data_loaded_into_ram(self):
        data = image()
        data[84:116] = inspect_image.PROGRAM_HEADER.pack(
            1,
            len(data),
            inspect_image.RAM_START,
            inspect_image.APP_START + 272,
            16,
            1024,
            6,
            4,
        )
        data.extend(bytes(16))
        report = inspect_image.inspect(data)
        self.assertEqual(report["flash_loaded_bytes"], 288)
        self.assertEqual(report["flash_end_exclusive"], "0x08007120")
        self.assertEqual(report["ram_static_bytes"], 1024)

    def test_overlapping_flash_segments_rejected(self):
        data = image()
        address = inspect_image.APP_START + 4
        data[84:116] = inspect_image.PROGRAM_HEADER.pack(
            1, 256, address, address, 8, 8, 4, 4
        )
        with self.assertRaisesRegex(ValueError, "Overlapping load segments"):
            inspect_image.inspect(data)

    def test_flash_budget_boundary(self):
        data = image()
        budget = inspect_image.FLASH_END - inspect_image.APP_START
        data.extend(bytes(budget - 272))
        struct.pack_into("<II", data, 52 + 16, budget, budget)
        self.assertEqual(
            inspect_image.inspect(data)["flash_end_exclusive"], "0x0800FC00"
        )
        data.append(0)
        struct.pack_into("<II", data, 52 + 16, budget + 1, budget + 1)
        with self.assertRaisesRegex(ValueError, "outside application flash/RAM"):
            inspect_image.inspect(data)

    def test_ram_stack_boundary(self):
        data = image()
        capacity = inspect_image.STACK_BOTTOM - inspect_image.RAM_START
        struct.pack_into("<I", data, 84 + 20, capacity)
        self.assertEqual(inspect_image.inspect(data)["ram_static_bytes"], capacity)
        struct.pack_into("<I", data, 84 + 20, capacity + 1)
        with self.assertRaisesRegex(ValueError, "reserved 2 KiB stack"):
            inspect_image.inspect(data)

    def test_generated_font_uses_existing_ascii_data(self):
        generated = generate_font.generate()
        self.assertIn("ts21_font[59][24]", generated)
        self.assertEqual(generated.count("  {"), 59)
        self.assertTrue(generated.isascii())


def hex_record(kind, payload=b"", address=0):
    data = bytes((len(payload), address >> 8, address & 0xFF, kind)) + bytes(payload)
    return ":" + (data + bytes((-sum(data) & 0xFF,))).hex().upper()


class HexImageTests(unittest.TestCase):
    def setUp(self):
        self.elf = image()
        self.payload = self.elf[256:]
        self.lines = [hex_record(4, b"\x08\x00")]
        self.lines.extend(
            hex_record(0, self.payload[offset : offset + 16], 0x7000 + offset)
            for offset in range(0, len(self.payload), 16)
        )
        self.lines.extend(
            [hex_record(5, inspect_image.APP_START.to_bytes(4, "big")), hex_record(1)]
        )

    def inspect_hex(self):
        return inspect_image.inspect(
            self.elf, ("\n".join(self.lines) + "\n").encode("ascii")
        )

    def test_matching_hex(self):
        report = self.inspect_hex()
        self.assertEqual(report["hex_loaded_bytes"], 272)
        self.assertEqual(report["status"], "layout-valid; hardware-unvalidated")
        self.assertEqual(len(report["hex_sha256"]), 64)

    def test_crlf_hex(self):
        report = inspect_image.inspect(
            self.elf, ("\r\n".join(self.lines) + "\r\n").encode("ascii")
        )
        self.assertEqual(report["hex_loaded_bytes"], 272)

    def test_optional_start_record(self):
        del self.lines[-2]
        self.assertEqual(self.inspect_hex()["hex_loaded_bytes"], 272)

    def test_bad_checksum_rejected(self):
        checksum = int(self.lines[1][-2:], 16) ^ 1
        self.lines[1] = self.lines[1][:-2] + f"{checksum:02X}"
        with self.assertRaisesRegex(ValueError, "checksum"):
            self.inspect_hex()

    def test_wrong_payload_with_valid_checksum_rejected(self):
        payload = bytearray(self.payload[:16])
        payload[0] ^= 1
        self.lines[1] = hex_record(0, payload, 0x7000)
        with self.assertRaisesRegex(ValueError, "do not match"):
            self.inspect_hex()

    def test_missing_payload_rejected(self):
        del self.lines[2]
        with self.assertRaisesRegex(ValueError, "do not match"):
            self.inspect_hex()

    def test_extra_payload_rejected(self):
        self.lines.insert(-2, hex_record(0, b"\x00", 0x7000 + len(self.payload)))
        with self.assertRaisesRegex(ValueError, "do not match"):
            self.inspect_hex()

    def test_missing_eof_rejected(self):
        self.lines.pop()
        with self.assertRaisesRegex(ValueError, "Missing HEX EOF"):
            self.inspect_hex()

    def test_content_after_eof_rejected(self):
        self.lines.append(hex_record(1))
        with self.assertRaisesRegex(ValueError, "after EOF"):
            self.inspect_hex()

    def test_overlapping_data_rejected(self):
        self.lines.insert(2, self.lines[1])
        with self.assertRaisesRegex(ValueError, "Overlapping"):
            self.inspect_hex()

    def test_out_of_order_data_rejected(self):
        self.lines[1], self.lines[2] = self.lines[2], self.lines[1]
        with self.assertRaisesRegex(ValueError, "out-of-order"):
            self.inspect_hex()

    def test_reserved_or_wrapping_addresses_rejected(self):
        for address in (0x6FF0, 0xFC00, 0xFFF8):
            with self.subTest(address=address), self.assertRaises(ValueError):
                self.lines[1] = hex_record(0, self.payload[:16], address)
                self.inspect_hex()

    def test_wrong_entry_rejected(self):
        self.lines[-2] = hex_record(5, (inspect_image.APP_START + 4).to_bytes(4, "big"))
        with self.assertRaisesRegex(ValueError, "HEX entry"):
            self.inspect_hex()

    def test_duplicate_start_rejected(self):
        self.lines.insert(-1, self.lines[-2])
        with self.assertRaisesRegex(ValueError, "start-address"):
            self.inspect_hex()

    def test_malformed_control_records_rejected(self):
        records = (
            hex_record(4, b"\x08"),
            hex_record(4, b"\x08\x00", 1),
            hex_record(4, b"\x20\x00"),
            hex_record(5, b"\x08\x00\x70"),
            hex_record(1, b"\x00"),
            hex_record(1, address=1),
            hex_record(2, b"\x08\x00"),
            hex_record(0, address=0x7000),
            ":01000001FD",
        )
        for record in records:
            with self.subTest(record=record), self.assertRaises(ValueError):
                self.lines[0] = record
                self.inspect_hex()

    def test_malformed_text_rejected(self):
        for data in (
            b"",
            b":",
            b":0",
            b":00000001FG\n",
            b" :00000001FF\n",
            b":00000001FF \n",
            b"\xFF",
        ):
            with self.subTest(data=data), self.assertRaises(ValueError):
                inspect_image.inspect(self.elf, data)


if __name__ == "__main__":
    unittest.main()
