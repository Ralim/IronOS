# Menu System

In this firmware for these soldering irons, all settings are adjustable on the device itself. This means a computer is **not** required to change any setting.

## Soldering mode

In this mode the iron works as you would expect, pressing either button will take you to a temperature change screen.
- Use each button to go up/down in temperature. Pressing both buttons exits the temperature menu (or wait 3 seconds and it will time out).
- Pressing both buttons or holding the rear button (`-/B`) will exit Soldering Mode.
- Holding the front button (`+/A`) will enter [Boost mode](/Documentation/Menu.md#boost-mode) (if enabled).

## Settings mode

This mode allows you to cycle through all the options and set custom values.
The menu is arranged so that the most often used settings are first.

- The rear button (`-/B`) cycles through the main options.
- The front button (`+/A`) changes the selected option.
- Note that settings are not saved until you exit the menu.
- If you idle on a setting (i.e., don't press any buttons), after 3 seconds, the screen scrolls a brief description (mini help guide).
- Enter submenus using the front button (`+/A`) if you are going to change it or wish to view it.
- Scrolling through the all options of a submenu will return you back to its entry location.

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
This is normally not needed unless you have an issue with your temperature or your tips are wearing out prematurely (tip is much higher temperature than displayed). Changing tip lengths does not necessarily mean a calibration is needed. If the method below is not followed closely, the iron could be worse than before calibration, repeat the method.
Some tips have an offset on their readings which causes issues and needs a [cold junction calibration](https://www.youtube.com/watch?v=AYblSfpKRUk&t=406s). To calibrate this out, perform the following steps:

1. Connect power to your iron
2. **Critical: make sure the tip & handle is at room temperature** (wait a good while after using the iron before calibration)
3. Using (`-/B`) scroll down to Advanced settings > press (`+/A`) to select > then (`-/B`) to scroll to Calibrate temperature? > press (`+/A`)
4. Important warning message is shown: *Affirm the tip & device are at room temperature before starting!* 
5. Press the (`+/A`) button to confirm (tip/handle are at ambient temperature).
6. Display shows **`....`** for a short time as the unit measures and compares the tip and handle temperatures.
7. The display then goes back to *TMP CAL* and displays your *Offset number*. You generally don't need this offset unless debugging issues.
8. After you see an Offset number, calibration is done, just exit the settings menu normally, use (`-/B`).
9. You're done, enjoy your iron!

Note: offsets are dependant on your tip, temperature sensor, and the MCU. It's the culmination of tolerances at rest. Typical values are 700-1000 range. This is only designed to be used at boot while cold (ambient room temperature), as temperatures drift apart as soon as power is connected. Doing this reading repeatedly could result in wide varience of the offset number and/or incorrect calibration.

### Boost mode

This allows you to change the front button (`+/A`) to become a boost button when you hold it for > 2 seconds. A boost button changes the soldering temperature for short periods. For example, when soldering a big joint and you need a much higher temperature, hold the (`+/A`) button down and it will temporarily increase the temperature to your 'boost' setting. When you release the button, the temperature will gradually go back to the normal set temperature.

The boost temperature is set in Soldering settings.
