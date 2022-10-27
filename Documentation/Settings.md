<!-- This is an automatically generated file. DO NOT EDIT. Edit gen_menu_docs.py instead -->

# IronOS Settings Menu

The below breaks down the menu's and what each setting means.
    
## Menu Categories

In the menu there are a few main categories that are used to keep the list manageable.

### Category: Power settings

Menu for settings related to power. Main settings to do with the input voltage.

### Category: Soldering settings

Settings for soldering mode, such as boost temps, the increment used when pressing buttons and if button locking is enabled.

### Category: Sleep mode

Settings to do with power saving, such as sleep mode, sleep temps, and shutdown modes.

### Category: User interface

User interface related settings, such as units.

### Category: Advanced settings

Advanced settings. Misc catchall for settings that don't fit anywhere else or settings that require some thought before use.

## Settings

These are all of the settings possible in the menu.
**Not all settings are visible for all devices.**
For example, the TS100 does not have USB-PD settings.

When using the device, if unsure you can pause (press nothing) on a setting and after a short delay help text will scroll across the screen.
This is the "on device help text".

### Setting: Power source

When the device is powered by a battery, this adjusts the low voltage threshold for when the unit should turn off the heater to protect the battery.

On device help text:

Set cutoff voltage to prevent battery over-drain. (DC 10V) (S=3.3V per cell, disable PWR limit)

### Setting: Sleep temp

Temperature the device will drop down to while asleep. Typically around halfway between off and soldering temperature.

On device help text:

Tip temperature while in "sleep mode"

### Setting: Sleep timeout

How long of a period without movement / button-pressing is required before the device drops down to the sleep temperature.

On device help text:

Interval before "sleep mode" starts (s=seconds | m=minutes)

### Setting: Shutdown timeout

How long of a period without movement / button-pressing is required before the device turns off the tip heater completely and returns to the main idle screen.

On device help text:

Interval before the iron shuts down (m=minutes)

### Setting: Motion sensitivity

Scale of how sensitive the device is to movement. Higher numbers == more sensitive. 0 == motion detection turned off.

On device help text:

0=off | 1=least sensitive | ... | 9=most sensitive

### Setting: Temperature unit

If the device shows temperatures in °C or °F.

On device help text:

C=Celsius | F=Fahrenheit

### Setting: Detailed idle screen

Should the device show an 'advanced' view on the idle screen. The advanced view uses text to show more details than the typical icons.

On device help text:

Display detailed info in a smaller font on idle screen

### Setting: Display orientation

If the display should rotate automatically or if it should be fixed for left- or right-handed mode.

On device help text:

R=right-handed | L=left-handed | A=automatic

### Setting: Boost temp

When the unit is in soldering mode. You can hold down the button at the front of the device to temporarily override the soldering temperature to this value. This SETS the temperature, it does not ADD to it.

On device help text:

Tip temperature used in "boost mode"

### Setting: Start-up behavior

When the device powers up, should it enter into a special mode. These settings set it to either start into soldering mode, sleeping mode or auto mode (Enters into soldering mode on the first movement).

On device help text:

O=off | S=heat to soldering temp | Z=standby at sleep temp until moved | R=standby, heat-off until moved

### Setting: Cooldown flashing

If the idle screen should blink the tip temperature for attention while the tip is over 50°C. Intended as a 'tip is still hot' warning.

On device help text:

Flash temperature reading at idle if tip is hot

### Setting: Calibrate CJC at next boot

Note:
If the difference between the target temperature and the measured temperature is less than 5°C, **calibration is NOT required at all**.

This is used to calibrate the offset between ADC and Op-amp of the tip **at next boot** (Ideally it has to be done at boot, before internal components get warm.). If the checkbox is set, the calibration will only be performed at the next boot. After a successful calibration the checkbox will be unchecked again! If you need to repeat the calibration however, you have to set the checkbox *again*, unplug your device and let it cool down to room/ambient temperature & power it up, ideally while it sits on the desk.

Also, the calibration will only take place if both of the following conditions are met:
- The tip must be installed.
- The temperature difference between tip and handle must be less than 10°C. (~ ambient / room temperature)

Otherwise, the calibration will be performed the next time the device is started and both conditions are met, unless the corresponding checkbox is unchecked.

Hence, never repeat the calibration in quick succession!

On device help text:

Calibrate tip Cold Junction Compensation at the next boot (not required if Delta T is < 5°C)

### Setting: Restore default settings

Resets all settings and calibrations to factory defaults. Does NOT erase custom user boot up logo's.

On device help text:

Reset default settings for this firmware ver.

### Setting: Calibrate input voltage

Enters an adjustment mode where you can gradually adjust the measured voltage to compensate for any unit-to-unit variance in the voltage sense resistors.

On device help text:

Start VIN calibration (long press to exit)

### Setting: Detailed solder screen

Should the device show an 'advanced' soldering view. This is a text-based view that shows more information at the cost of no nice graphics.

On device help text:

Display detailed info in a smaller font on soldering screen

### Setting: Scrolling speed

How fast the description text scrolls when hovering on a menu. Faster speeds may induce tearing, but allow reading the whole description faster.

On device help text:

Speed info text scrolls past at (S=slow | F=fast)

### Setting: QC voltage

This adjusts the maximum voltage the QC negotiation will adjust to. Does NOT affect USB-PD. Should be set safely based on the current rating of your power supply.

On device help text:

Max QC voltage the iron should negotiate for

### Setting: PD timeout

How long until firmware stops trying to negotiate for USB-PD and tries QC instead. Longer times may help dodgy / old PD adapters, faster times move onto PD quickly. Units of 100ms. Recommended to keep small values.

On device help text:

PD negotiation timeout in 100ms steps for compatibility with some QC chargers

### Setting: Power limit

Allows setting a custom wattage for the device to aim to keep the AVERAGE power below. The unit can't control its peak power no matter how you set this. (Except for MHP30 which will regulate nicely to this). If USB-PD is in use, the limit will be set to the lower of this and the supplies advertised wattage.

On device help text:

Maximum power the iron can use (W=watt)

### Setting: Swap + - keys

Swaps which button increments and decrements on temperature change screens.

On device help text:

Reverse assignment of buttons for temperature adjustment

### Setting: Temp change short

Factor by which the temperature is changed with a quick press of the buttons.

On device help text:

Temperature-change-increment on short button press

### Setting: Temp change long

Factor by which the temperature is changed with a hold of the buttons.

On device help text:

Temperature-change-increment on long button press

### Setting: Power pulse

Enables and sets the wattage of the power pulse. Power pulse causes the device to briefly turn on the heater to draw power to avoid power banks going to sleep.

On device help text:

Intensity of power of keep-awake-pulse (watt)

### Setting: Hall sensor sensitivity

If the unit has a hall effect sensor (Pinecil), this adjusts how sensitive it is at detecting a magnet to put the device into sleep mode.

On device help text:

Sensitivity to magnets (0=off | 1=least sensitive | ... | 9=most sensitive)

### Setting: Allow locking buttons

If locking the buttons against accidental presses is enabled.

On device help text:

While soldering, hold down both buttons to toggle locking them (D=disable | B=boost mode only | F=full locking)

### Setting: Minimum voltage

When powered by a battery, this adjusts the minimum voltage per cell before shutdown. (This is multiplied by the cell count.)

On device help text:

Minimum allowed voltage per battery cell (3S: 3 - 3.7V | 4-6S: 2.4 - 3.7V)

### Setting: Anim. loop

Should the menu animations loop. Only visible if the animation speed is not set to "Off"

On device help text:

Loop icon animations in main menu

### Setting: Anim. speed

How fast should the menu animations loop, or if they should not loop at all.

On device help text:

Pace of icon animations in menu (O=off | S=slow | M=medium | F=fast)

### Setting: Power pulse delay

Adjusts the time interval between power pulses. Longer gaps reduce undesired heating of the tip, but needs to be fast enough to keep your power bank awake.

On device help text:

Delay before keep-awake-pulse is triggered (x 2.5s)

### Setting: Power pulse duration

How long should the power pulse go for. Some power banks require seeing the power draw be sustained for a certain duration to keep awake. Should be kept as short as possible to avoid wasting power / undesired heating of the tip.

On device help text:

Keep-awake-pulse duration (x 250ms)

### Setting: Language:  EN     English

Changes the device language on multi-lingual builds.

On device help text:

Current firmware language

### Setting: Screen brightness

Display brightness. Higher values age the OLED faster due to burn-in. (However, it is notable that most of these screens die from other causes first.)

On device help text:

Adjust the OLED screen brightness

### Setting: Invert screen

Inverts the entire OLED.

On device help text:

Invert the OLED screen colors

### Setting: Boot logo duration

Sets the duration for the boot logo (s=seconds).

On device help text:

Set Boot logo duration (off | s=seconds | infinity)
