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
			"id": "SleepingSimpleString",
			"maxLen": 4
		},
		{
			"id": "SleepingAdvancedString",
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
			"id": "SolderingAdvancedPowerPrompt",
			"maxLen": null
		},
		{
			"id": "OffString",
			"maxLen": 3
		}
	],
	"messagesWarn": [
		{
			"id": "ResetOKMessage"
		},
		{
			"id": "SettingsResetMessage"
		},
		{
			"id": "NoAccelerometerMessage"
		},
		{
			"id": "NoPowerDeliveryMessage"
		},
		{
			"id": "LockingKeysString"
		},
		{
			"id": "UnlockingKeysString"
		},
		{
			"id": "WarningKeysLockedString"
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
		},
		{
			"id": "SettingMediumChar",
			"len": 1
		},
		{
			"id": "SettingOffChar",
			"len": 1
		},
		{
			"id": "SettingStartSolderingChar",
			"len": 1
		},
		{
			"id": "SettingStartSleepChar",
			"len": 1
		},
		{
			"id": "SettingStartSleepOffChar",
			"len": 1
		},
		{
			"id": "SettingStartNoneChar",
			"len": 1
		},
		{
			"id": "SettingSensitivityOff",
			"len": 1
		},
		{
			"id": "SettingSensitivityLow",
			"len": 1
		},
		{
			"id": "SettingSensitivityMedium",
			"len": 1
		},
		{
			"id": "SettingSensitivityHigh",
			"len": 1
		},
		{
			"id": "SettingLockDisableChar",
			"len": 1,
			"default": "D"
		},
		{
			"id": "SettingLockBoostChar",
			"len": 1,
			"default": "B"
		},
		{
			"id": "SettingLockFullChar",
			"len": 1,
			"default": "F"
		},
		{
			"id": "SettingNAChar",
			"len": 3,
			"default": "N/A"
		}
	],
	"menuGroups": [
		{
			"id": "PowerMenu",
			"maxLen": 5,
			"maxLen2": 11
		},
		{
			"id": "SolderingMenu",
			"maxLen": 5,
			"maxLen2": 11
		},
		{
			"id": "PowerSavingMenu",
			"maxLen": 5,
			"maxLen2": 11
		},
		{
			"id": "UIMenu",
			"maxLen": 5,
			"maxLen2": 11
		},
		{
			"id": "AdvancedMenu",
			"maxLen": 5,
			"maxLen2": 11
		}
	],
	"menuOptions": [
		{
			"id": "DCInCutoff",
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
			"id": "QCMaxVoltage",
			"maxLen": 8,
			"maxLen2": 16
		},
		{
			"id": "PowerLimit",
			"maxLen": 5,
			"maxLen2": 11
		},
		{
			"id": "ReverseButtonTempChange",
			"maxLen": 6,
			"maxLen2": 16
		},
		{
			"id": "TempChangeShortStep",
			"maxLen": 8,
			"maxLen2": 16
		},
		{
			"id": "TempChangeLongStep",
			"maxLen": 6,
			"maxLen2": 16
		},
		{
			"id": "PowerPulsePower",
			"maxLen": 6,
			"maxLen2": 16
		},
		{
			"id": "HallEffSensitivity",
			"maxLen": 6,
			"maxLen2": 8
		},
		{
			"id": "LockingMode",
			"maxLen": 6,
			"maxLen2": 13
		},
		{
			"id": "MinVolCell",
			"maxLen": 4,
			"maxLen2": 9
		},
		{
			"id": "AnimLoop",
			"maxLen": 6,
			"maxLen2": 13
		},
		{
			"id": "AnimSpeed",
			"maxLen": 6,
			"maxLen2": 13
		},
		{
			"id": "PowerPulseWait",
			"maxLen": 6,
			"maxLen2": 13
		},
		{
			"id": "PowerPulseDuration",
			"maxLen": 6,
			"maxLen2": 13
		},
		{
			"id": "LanguageSwitch",
			"maxLen": 7,
			"maxLen2": 15
		},
		{
			"id": "QCMode",
			"maxLen": 8,
			"maxLen2": 16
		}
	]
}
