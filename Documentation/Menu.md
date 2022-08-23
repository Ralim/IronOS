# Menu System

In this firmware for these soldering irons, all settings are adjustable on the device itself. This means you do **not** require a computer to change any settings.

## Soldering mode

In this mode the iron works as you would expect, pressing either button will take you to a temperature change screen.
Use each button to go up and down in temperature. Pressing both buttons will exit you from the temperature menu (or wait 3 seconds and it will time out).
Pressing both buttons or holding the button near the USB will exit the soldering mode.
Holding the button at the front of the iron will enter boost mode (if enabled).

## Settings Menu

This menu allows you to cycle through all the options and set their values.
The button near the USB cycles through the options, and the one near the tip changes the selected option.
Note that settings are not saved until you exit the menu.
If you leave the unit alone (i.e., don't press any buttons) on a setting, after 3 seconds the screen will scroll a rough description of the setting.

The menu is arranged so that the most often used settings are first.
With submenus being selected to enter using the front button (as if you were going to change the setting).
Scrolling through the submenu will return you back to its entry location after you scroll through all of the options.

### Calibrating input voltage

Due to the tolerance on the resistors used for the input voltage divider, some irons can be up to 0.6 V out on the voltage measurement.
Please calibrate your iron if you have any issues with the cutoff voltage. This calibration is not required if you have no issues.
Note that cutoff messages can also be triggered by using a power supply that is too weak and fails under the load of the iron.

To calibrate your iron:

1. Measure the input voltage with a multimeter and note it down.
2. Connect the input to your iron.
3. Enter the settings menu
4. Under the Advanced submenu
5. Select the calibrate voltage option
6. Use the front and back buttons to adjust the displayed voltage to minimize the error to your original measurement
7. Press both buttons at the same time to Save and Exit to the menu

### Calibrating tip offset (Set tip model)
This is normally not needed unless you have an issue with your temperature or your tips are wearing out prematurely. Changing tip lengths does not necessarily mean a calibration is needed. If the method below is not followed, the iron could be worse than before calibration, repeat the method.
Some tips will have an offset on their readings, to calibrate this out, perform the following steps:

1. Connect power to your iron
2. **Critical: make sure the tip & handle is at room temperature** (wait a good while after using the iron before calibration)
3. Using (`-/B`) scroll down to Advanced settings > press (`+/A`) to select > then (`-/B`) to scroll to Calibrate temperature? > press (`+/A`) until you see ...
4. The display will scroll a warning message to Check that the tip is at room/ambient temperature!
5. Press the `+` button to confirm
6. The display shows "...." for a short time as the unit measures the tip temperature and the handle temperature and compares them.
7. The display will then go back to *TMP CAL* and it will display your Offset number. You generally don't need it unless debugging issues.
8. Calibration is done, just exit the settings menu as normal
9. You're done, enjoy your iron!

Note: offsets are dependant on your tip, temperature sensor, and the MCU. It's the culmination of tolerances at rest. Typical values are 700-1000 range. This is only designed to be used at boot while cold (ambient room temperature), as temperatures drift apart as soon as power is connected. Doing this reading repeatedly could result in wide varience of the offset number and/or incorrect calibration. 

### Boost mode

This allows you to change the front button `+` (one near the tip) to become a boost button when you hold it for > 2 seconds. This allows you to set this button to change the soldering temperature for short periods. For example, when soldering a big joint and you want to boost the temperature a bit.

The boost temperature is set in the settings menu.
