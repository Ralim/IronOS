var def =
{
	"messages": [
		{
			"id": "SettingsCalibrationWarning"
		}, 
		{
			"id": "SettingsResetWarning"
		}, 
		{
			"id": "UVLOWarningString",
			"maxLen": 8
		}, 
		{
			"id": "UndervoltageString",
			"maxLen": 16
		}, 
		{
			"id": "InputVoltageString",
			"maxLen": 11,
			"note": "Preferably end with a space"
		}, 
		{
			"id": "WarningTipTempString",
			"maxLen": 12,
			"note": "Preferably end with a space"
		}, 
		{
			"id": "BadTipString",
			"maxLen": 8
		}, 
		{
			"id": "SleepingSimpleString",
			"maxLen": 4
		}, 
		{
			"id": "SleepingAdvancedString",
			"maxLen": 16
		}, 
		{
			"id": "WarningSimpleString",
			"maxLen": 4
		}, 
		{
			"id": "WarningAdvancedString",
			"maxLen": 16
		}, 
		{
			"id": "SleepingTipAdvancedString",
			"maxLen": 6
		}, 
		{
			"id": "IdleTipString",
			"lenSum": 
			{
				"fields": ["IdleTipString", "IdleSetString"],
				"maxLen": 10
			}
		}, 
		{
			"id": "IdleSetString",
			"lenSum": 
			{
				"fields": ["IdleTipString", "IdleSetString"],
				"maxLen": 10
			},
			"note": "Preferably start with a space"
		}, 
		{
			"id": "TipDisconnectedString",
			"maxLen": 16
		}, 
		{
			"id" :"SolderingAdvancedPowerPrompt",
			"maxLen": null
		},
		{
			"id" :"OffString",
			"maxLen": 3
		}
	],
	"characters": [
		{
			"id": "SettingRightChar",
			"len": 1
		},
		{
			"id": "SettingLeftChar",
			"len": 1
		},
		{
			"id": "SettingAutoChar",
			"len": 1
		},
		{
			"id": "SettingFastChar",
			"len": 1
		},
		{
			"id": "SettingSlowChar",
			"len": 1
		}
	],
	"menuGroups": [
		{
			"id": "SolderingMenu",
			"maxLen": 11
		},
		{
			"id": "PowerSavingMenu",
			"maxLen": 11
		},
		{
			"id": "UIMenu",
			"maxLen": 11
		},
		{
			"id": "AdvancedMenu",
			"maxLen": 11
		}
	],
	"menuOptions": [
		{
			"id": "PowerSource",
			"maxLen": 5,
			"maxLen2": 11
		},
		{
			"id": "SleepTemperature",
			"maxLen": 4,
			"maxLen2": 9
		},
		{
			"id": "SleepTimeout",
			"maxLen": 4,
			"maxLen2": 9
		},
		{
			"id": "ShutdownTimeout",
			"maxLen": 5,
			"maxLen2": 11
		},
		{
			"id": "MotionSensitivity",
			"maxLen": 6,
			"maxLen2": 13
		},
		{
			"id": "TemperatureUnit",
			"maxLen": 6,
			"maxLen2": 13
		},
		{
			"id": "AdvancedIdle",
			"maxLen": 6,
			"maxLen2": 13
		},
		{
			"id": "DisplayRotation",
			"maxLen": 6,
			"maxLen2": 13
		},
		{
			"id": "BoostEnabled",
			"maxLen": 6,
			"maxLen2": 13
		},
		{
			"id": "BoostTemperature",
			"maxLen": 4,
			"maxLen2": 9
		},
		{
			"id": "AutoStart",
			"maxLen": 6,
			"maxLen2": 13
		},
		{
			"id": "CooldownBlink",
			"maxLen": 6,
			"maxLen2": 13
		},
		{
			"id": "TemperatureCalibration",
			"maxLen": 8,
			"maxLen2": 16
		},
		{
			"id": "SettingsReset",
			"maxLen": 8,
			"maxLen2": 16
		},
		{
			"id": "VoltageCalibration",
			"maxLen": 8,
			"maxLen2": 16
		},
		{
			"id": "AdvancedSoldering",
			"maxLen": 6,
			"maxLen2": 13
		},
		{
			"id": "ScrollingSpeed",
			"maxLen": 6,
			"maxLen2": 11
		},
		{
			"id": "PidSettingsP",
			"maxLen": 6,
			"maxLen2": 11,
			"feature": "PIDSETTINGS"
		},
		{
			"id": "PidSettingsI",
			"maxLen": 6,
			"maxLen2": 11,
			"feature": "PIDSETTINGS"
		},
		{
			"id": "PidSettingsD",
			"maxLen": 6,
			"maxLen2": 11,
			"feature": "PIDSETTINGS"
		}
	]
}