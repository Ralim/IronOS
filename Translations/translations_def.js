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
		},
		{
			"id": "WarningThermalRunaway"
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
			"maxLen2": 11,
			"description":"When device is powered by a battery, adjusts the low voltage threshold for when the unit turns off the heater to protect the battery."
		},
		{
			"id": "SleepTemperature",
			"maxLen": 4,
			"maxLen2": 9,
			"description":"Temperature the device will drop down to while asleep. Typically around halfway between off and soldering temperature."
		},
		{
			"id": "SleepTimeout",
			"maxLen": 4,
			"maxLen2": 9,
			"description":"How long after inactivity the device drops down to the sleep temperature."
		},
		{
			"id": "ShutdownTimeout",
			"maxLen": 5,
			"maxLen2": 11,
			"description":"How long after the unit enters sleep mode before it turns off the heater and exists soldering mode."
		},
		{
			"id": "MotionSensitivity",
			"maxLen": 6,
			"maxLen2": 13,
			"description":"Scale of how sensitive the device is to movement. Higher numbers == more sensitive. 0 == motion detection turned off."
		},
		{
			"id": "TemperatureUnit",
			"maxLen": 6,
			"maxLen2": 13,
			"description":"If the device shows temperatures in C or F"
		},
		{
			"id": "AdvancedIdle",
			"maxLen": 6,
			"maxLen2": 13,
			"description":"Should the device show an 'advanced' view on the idle screen. Advanced view uses text to show more details than the typical icons."
		},
		{
			"id": "DisplayRotation",
			"maxLen": 6,
			"maxLen2": 13,
			"description":"If the display should rotate automatically or if it should be fixed into Left or Right handed mode."
		},
		{
			"id": "BoostTemperature",
			"maxLen": 4,
			"maxLen2": 9,
			"description":"When the unit is in soldering mode. You can hold down the button at the front of the device to temporarily override the soldering temperature to this value. This SETS the temperature, it does not ADD to it."
		},
		{
			"id": "AutoStart",
			"maxLen": 6,
			"maxLen2": 13,
			"description":"When the device powers up, should it enter into a special mode. These settings set it to either start into: Soldering mode, sleeping mode, Auto mode (Enters into soldering mode on first movement)."
		},
		{
			"id": "CooldownBlink",
			"maxLen": 6,
			"maxLen2": 13,
			"description":"If the idle screen should blink the tip temperature for attention when the device tip is over 50C. Intended as a 'tip is hot' warning."
		},
		{
			"id": "TemperatureCalibration",
			"maxLen": 8,
			"maxLen2": 16,
			"description":"Used to calibrate the ADC+Op-amp offsets for the tip. This calibration must be performed when the tip temperature and the handle temperature are equal. Generally not required unless your device is reading more than 5C off target."
		},
		{
			"id": "SettingsReset",
			"maxLen": 8,
			"maxLen2": 16,
			"description":"Resets all settings and calibrations to factory defaults. Does NOT erase custom user bootup logo's"
		},
		{
			"id": "VoltageCalibration",
			"maxLen": 8,
			"maxLen2": 16,
			"description":"Enters an adjustment mode where you can gradually adjust the measured voltage to compensate for any unit-to-unit variance in the voltage sense resistors"
		},
		{
			"id": "AdvancedSoldering",
			"maxLen": 6,
			"maxLen2": 13,
			"description":""
		},
		{
			"id": "ScrollingSpeed",
			"maxLen": 6,
			"maxLen2": 11,
			"description":"How fast the description text scrolls when hovering on a menu. Faster speeds may induce tearing but allow reading the whole description faster."
		},
		{
			"id": "QCMaxVoltage",
			"maxLen": 8,
			"maxLen2": 16,
			"description":"This adjusts the maximum voltage the QC negotiation will adjust to. Does NOT affect USB-PD. Should be set safely based on the current rating of your power supply."
		},
		{
			"id": "PDNegTimeout",
			"maxLen": 8,
			"maxLen2": 16,
			"description":"How long until firmware stops trying to negotiate for USB-PD and tries QC instead. Longer times may help dodgy/old PD adapters, faster times move onto PD quickly. Units of 100ms. Reccomended to keep small."
		},

		{
			"id": "PowerLimit",
			"maxLen": 5,
			"maxLen2": 11,
			"description":"Allows setting a custom Wattage for the iron to aim to keep the AVERAGE power below. The Iron cant control its peak power no matter how you set this. (Except for MHP30 which will regulate nicely to this). If USB-PD is in use, the limit will be set to the lower of this and the supplies advertised wattage."
		},
		{
			"id": "ReverseButtonTempChange",
			"maxLen": 6,
			"maxLen2": 16,
			"description":"Swaps which button increments and decrements the temperature on temperature change screens."
		},
		{
			"id": "TempChangeShortStep",
			"maxLen": 8,
			"maxLen2": 16,
			"description":"How much the temperature is increased with a quick press of the buttons."
		},
		{
			"id": "TempChangeLongStep",
			"maxLen": 6,
			"maxLen2": 16,
			"description":"How much the temperature is increased with a long hold of the buttons."
		},
		{
			"id": "PowerPulsePower",
			"maxLen": 6,
			"maxLen2": 16,
			"description":"Enables and sets the wattage of the power pulse. Power pulse causes the iron to briefly turn on the heater to draw power to avoid power banks going to sleep."
		},
		{
			"id": "HallEffSensitivity",
			"maxLen": 6,
			"maxLen2": 8,
			"description":"If unit has a hall effect (Pinecil), how sensitive it is at detecting a magnet to put the iron into sleep mode."
		},
		{
			"id": "LockingMode",
			"maxLen": 6,
			"maxLen2": 13,
			"description":"If the locking mode for the buttons is enabled. Allows locking the buttons from accidental presses."
		},
		{
			"id": "MinVolCell",
			"maxLen": 4,
			"maxLen2": 9,
			"description":"When powered by a battery, sets the minimum voltage per cell that is allowed. This is multiplied by the cell count."
		},
		{
			"id": "AnimLoop",
			"maxLen": 6,
			"maxLen2": 13,
			"description":"Should the menu animations loop."
		},
		{
			"id": "AnimSpeed",
			"maxLen": 6,
			"maxLen2": 13,
			"description":"How fast should the menu animations loop."
		},
		{
			"id": "PowerPulseWait",
			"maxLen": 6,
			"maxLen2": 13,
			"description":"How long of a time interval between power pulses."
		},
		{
			"id": "PowerPulseDuration",
			"maxLen": 6,
			"maxLen2": 13,
			"description":"How long should the power pulse go for."
		},
		{
			"id": "LanguageSwitch",
			"maxLen": 7,
			"maxLen2": 15,
			"description":"Changes the device language on multi-lingual builds"
		},
		{
			"id": "Brightness",
			"maxLen": 7,
			"maxLen2": 15,
			"description":"Display brightness. Higher values age the OLED faster but are brighter."
		},
		{
			"id": "ColourInversion",
			"maxLen": 7,
			"maxLen2": 15,
			"description":"Inverts the entire OLED."
		}
	]
}
