#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import json
from config_parser import parse_config


def test_parser():
    """
    Test the configuration parser on all supported models
    """
    # List of models to test
    models = [
        "TS100",
        "TS80",
        "TS80P",
        "TS101",
        "Pinecil",
        "Pinecilv2",
        "S60",
        "S60P",
        "MHP30",
    ]

    success_count = 0
    failed_models = []

    for model in models:
        print(f"Testing model: {model}")
        try:
            # Try to parse the configuration for this model
            config = parse_config(model)

            # Print number of config items found
            print(f"  Found {len(config)} configuration items")

            # Print a few sample values if available
            if config:
                sample_keys = list(config.keys())[:5]
                print("  Sample values:")
                for key in sample_keys:
                    print(f"    {key}: {config[key]}")

            # Check for key configuration parameters
            important_keys = [
                "SOLDERING_TEMP",
                "SLEEP_TEMP",
                "BOOST_TEMP",
                "PID_POWER_LIMIT",
                "MAX_POWER_LIMIT",
            ]

            missing_keys = [key for key in important_keys if key not in config]
            if missing_keys:
                print(f"  Warning: Missing important keys: {', '.join(missing_keys)}")

            success_count += 1
            print("  Success!")

        except Exception as e:
            print(f"  Failed: {str(e)}")
            failed_models.append((model, str(e)))

        print("-" * 40)

    # Print summary
    print(f"\nSummary: {success_count}/{len(models)} models parsed successfully")
    if failed_models:
        print("Failed models:")
        for model, error in failed_models:
            print(f"  {model}: {error}")
    else:
        print("All models parsed successfully!")


if __name__ == "__main__":
    # If a specific model is provided as command line argument, test only that one
    if len(sys.argv) > 1:
        model = sys.argv[1]
        try:
            config = parse_config(model)
            print(json.dumps(config, indent=2))
        except Exception as e:
            print(f"Error parsing {model}: {e}", file=sys.stderr)
            sys.exit(1)
    else:
        # Otherwise, run the full test suite
        test_parser()
