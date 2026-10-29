#!/usr/bin/env python3
"""
settings_parser.py - Functions for parsing settings values and expressions
"""

import sys
import re
from typing import Dict, Any, Optional

# Import from local modules
from .settings_model import Settings
from .settings_util import resolve_expression


def process_default_values(
    settings: Settings, bsp_config: Optional[Dict[str, Any]] = None, debug: bool = False
) -> None:
    """Process and resolve default values that are expressions or refer to BSP configs

    Args:
        settings: Settings object with entries and values
        bsp_config: BSP configuration values (optional)
        debug: Print debug information
    """
    # Create a dictionary of values to use for resolving expressions
    # First add all values that are already integers
    values_dict = {}
    for i, value in enumerate(settings.values):
        if isinstance(value, int):
            values_dict[settings.entries[i].name] = value

    # If we have BSP config, add those values too
    if bsp_config:
        values_dict.update(bsp_config)
        if debug:
            print(f"Added {len(bsp_config)} values from BSP config")

    # Handle special cases and defaults for BSP config values
    special_cases = {
        "BoostTemperature": lambda cfg: cfg.get("BOOST_TEMP", 450) if cfg else 450,
        "SleepTemperature": lambda cfg: cfg.get("SLEEP_TEMP", 150) if cfg else 150,
        "SleepTimeout": lambda cfg: cfg.get("SLEEP_TIMEOUT", 10) if cfg else 10,
        "ShutdownTimeout": lambda cfg: cfg.get("SHUTDOWN_TIMEOUT", 10) if cfg else 10,
        "MotionSensitivity": lambda cfg: cfg.get("SENSITIVITY", 0) if cfg else 0,
        "TemperatureUnit": lambda cfg: (
            0 if cfg and cfg.get("TEMP_UNIT", "C") == "C" else 1
        ),
        "DisplayRotation": lambda cfg: cfg.get("ORIENTATION", 0) if cfg else 0,
        "CooldownBlink": lambda cfg: (
            1 if cfg and cfg.get("COOLING_BLINK", "enabled") == "enabled" else 0
        ),
        "ScrollingSpeed": lambda cfg: cfg.get("SCROLLSPEED", 0) if cfg else 0,
        "LockingMode": lambda cfg: cfg.get("LOCK_MODE", 0) if cfg else 0,
        "MinVolCell": lambda cfg: cfg.get("VOLTAGE_MIN", 30) if cfg else 30,
        "QCIdleVoltage": lambda cfg: cfg.get("QC_VOLTAGE", 90) if cfg else 90,
        "PDNegTimeout": lambda cfg: cfg.get("PD_TIMEOUT", 5) if cfg else 5,
        "AnimLoop": lambda cfg: (
            1 if cfg and cfg.get("ANIMATION_LOOP", "enabled") == "enabled" else 0
        ),
        "AnimSpeed": lambda cfg: cfg.get("ANIMATION_SPEED", 40) if cfg else 40,
        "AutoStart": lambda cfg: (
            0 if cfg and cfg.get("AUTOSTART_MODE", "none") == "none" else 1
        ),
        "ShutdownTime": lambda cfg: cfg.get("AUTO_SHUTDOWN_TIME", 30) if cfg else 30,
        "CalibrateInfo": lambda cfg: (
            1 if cfg and cfg.get("TIP_CALIBRATION_INFO", "on") == "on" else 0
        ),
        "PowerPulse": lambda cfg: (
            1 if cfg and cfg.get("POWER_PULSE", "enabled") == "enabled" else 0
        ),
        "PowerPulseWait": lambda cfg: cfg.get("POWER_PULSE_WAIT", 2) if cfg else 2,
        "PowerPulseDuration": lambda cfg: (
            cfg.get("POWER_PULSE_DURATION", 1) if cfg else 1
        ),
    }

    # Resolve special cases if BSP config is available
    for i, entry in enumerate(settings.entries):
        if entry.name in special_cases and not isinstance(settings.values[i], int):
            settings.values[i] = special_cases[entry.name](bsp_config)
            if debug:
                print(f"Applied special case for {entry.name}: {settings.values[i]}")
            values_dict[entry.name] = settings.values[i]

    # Now resolve remaining expressions
    changed = True
    max_passes = 10  # Limit the number of passes to avoid infinite loops with circular dependencies
    pass_count = 0

    while changed and pass_count < max_passes:
        changed = False
        pass_count += 1
        if debug:
            print(f"Pass {pass_count} resolving expressions")

        for i, value in enumerate(settings.values):
            if not isinstance(value, int):
                try:
                    resolved = resolve_expression(value, values_dict, debug)
                    if debug:
                        print(
                            f"Resolved {settings.entries[i].name} from '{value}' to {resolved}"
                        )
                    settings.values[i] = resolved
                    values_dict[settings.entries[i].name] = resolved
                    changed = True
                except Exception as e:
                    if debug:
                        print(
                            f"Failed to resolve {settings.entries[i].name} = '{value}': {e}"
                        )

    # Check if any values are still unresolved
    unresolved = []
    for i, value in enumerate(settings.values):
        if not isinstance(value, int):
            unresolved.append(f"{settings.entries[i].name} = '{value}'")

    if unresolved:
        print("\nWarning: Could not resolve some expressions:")
        for expr in unresolved:
            print(f"  {expr}")
        print("\nUsing default value of 0 for unresolved settings")

        # Set unresolved values to 0
        for i, value in enumerate(settings.values):
            if not isinstance(value, int):
                settings.values[i] = 0
