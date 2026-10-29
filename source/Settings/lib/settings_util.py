#!/usr/bin/env python3
"""
settings_util.py - Utility functions for IronOS settings management
"""

import sys
import re
from typing import Dict, Any


def get_base_address(model_code: str) -> int:
    """Get the base address for the given model code

    Args:
        model_code: The model code of the device (e.g., 'TS100', 'Pinecilv2')

    Returns:
        The base address for the settings in flash memory
    """
    base_addresses = {
        "TS100": (0x08000000 + (63 * 1024)),
        "TS80": (0x08000000 + (63 * 1024)),
        "TS80P": (0x08000000 + (63 * 1024)),
        "TS101": (0x08000000 + (127 * 1024)),
        "Pinecil": (0x08000000 + (127 * 1024)),
        "Pinecilv2": 0x23000000 + (1023 * 1024),
        "S60": (0x08000000 + (63 * 1024)),
        "S60P": (0x08000000 + (63 * 1024)),
        "MHP30": 0x08000000 + (127 * 1024),
    }
    # If the model code is not found, exit with an error
    if model_code not in base_addresses:
        print(f"Error: Model code '{model_code}' is not recognized.")
        sys.exit(1)
    return base_addresses[model_code]


def resolve_expression(
    expression: str, values: Dict[str, Any], debug: bool = False
) -> int:
    """Resolve a mathematical expression with variable substitution

    Args:
        expression: String expression like "100 + x / 2"
        values: Dictionary of variable values
        debug: Print debug information

    Returns:
        Resolved integer value
    """
    if isinstance(expression, (int, float)):
        return int(expression)

    if not isinstance(expression, str):
        raise ValueError(f"Invalid expression type: {type(expression)}")

    # Replace variable references with their values
    result_expr = expression

    # Find all variable references in the expression
    var_refs = re.findall(r"[A-Za-z_][A-Za-z0-9_]*", expression)

    if debug:
        print(f"Expression: {expression}")
        print(f"Found variables: {var_refs}")

    # Replace each variable with its value
    for var in var_refs:
        if var in values:
            # Make sure we replace whole words only (not parts of other words)
            # Using word boundaries in regex
            result_expr = re.sub(r"\b" + var + r"\b", str(values[var]), result_expr)
            if debug:
                print(f"Replaced {var} with {values[var]}")
        else:
            if debug:
                print(f"Warning: Variable {var} not found in values dictionary")

    if debug:
        print(f"Final expression: {result_expr}")

    try:
        # Evaluate the expression
        # Using eval is generally not recommended for security reasons,
        # but in this controlled environment with no user input, it's acceptable
        result = eval(result_expr)
        return int(result)
    except Exception as e:
        print(f"Error evaluating expression '{expression}' -> '{result_expr}': {e}")
        return 0
