# Menu System

In this firmware for these soldering irons, all settings are adjustable on the device itself. This means a computer is **not** required to change any setting.

## Soldering mode

In this mode the iron works as you would expect, pressing either button will take you to a temperature change screen.
- Use each button to go up/down in temperature. Pressing both buttons exits the temperature menu (or wait 3 seconds and it will time out).
- Pressing both buttons or holding the rear button (`-/B`) will exit Soldering Mode.
- Holding the front button (`+/A`) will enter [Boost mode](https://ralim.github.io/IronOS/Menu/#boost-mode) (if enabled).

## Settings mode

This mode allows you to cycle through all the options and set custom values.
The menu is arranged so that the most often used settings are first.

- The rear button (`-/B`) cycles through the main options. (declines i.e. Additional warning to proceed.)
- The front button (`+/A`) either enters a submenu or changes the selected option. (accepts i.e. Additional warning to proceed.)
- If the device is unplugged before exiting the main menu settings will not be saved.
- To exit the menu, either continue to press (`-/B`) or hold it until the idle screen is reached. Alternatively, you could press (`-/A`) & (`-/B`) simultaneously to exit the submenu and once more to exit the main menu.
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

### Calibrate Tip CJC
This calibrates the [Cold Junction Compensation](https://ralim.github.io/IronOS/Temperature/) *(CJC)* for the tip. This is normally not needed unless you have an issue with tip temperature or your tips are wearing out prematurely. Changing tip lengths does not necessarily mean a calibration is needed. Check first that your tips are not defective and measured resistance is close to specifications *[Pinecil / TS100 short tips **6.2 Ω**, long tips **8 Ω**, TS80/P ~**4.5 Ω**]*.

What this is for:<br>
Some tips have an offset on their readings which causes issues, i.e. The actual temperature of the tip is much higher than displayed. Follow the steps below to calibrate this.

Caution:<br>
If the method below is not followed, the iron could be worse than before calibration. If you need to repeat the method, first unplug and let the handle/PCB cool down to room temperature.

1. Connect power to your device.
2. Go to **`Advanced Settings`** using (`-/B`) and press (`+/A`) to select it. Use (`-/B`) to scroll to **`Calibrate CJC at next boot`** and confirm with (`+/A`).
3. Accept the *'warning text'* with (`+/A`).
3. Exit the settings menu as usual by pressing and holding (`-/B`).
4. Unplug you device.
5. **Critical: Make sure a tip is attached & wait until the tip & handle are at room temperature.** (Wait a reasonable amount of time after having used the device.)
6. Power the device and ideally keep it out of your hands (You know it might get warm.).
7. The display shows **`calibrating ....`** for a short time while the device measures and compares the tip and handle voltages.
8. **`Calibration done!`** is displayed for 3 seconds. The new offset value can later be viewed in the **`Debug menu`**.
9. Calibration is done and the device proceeds booting.

Note: offsets are dependant on your tip, temperature sensor, and the MCU. It's the culmination of tolerances at rest. Typical values are 700-1000 range. This is only designed to be used at boot while cold (ambient / room temperature), as temperatures drift apart as soon as power is connected. Doing this reading repeatedly could result in wide varience of the offset number and/or incorrect calibration.

### Boost mode

This allows you to change the front button (`+/A`) to become a boost button when you hold it for > 2 seconds. A boost button changes the soldering temperature for short periods. For example, when soldering a big joint and you need a much higher temperature, hold the (`+/A`) button down and it will temporarily increase the temperature to your 'boost' setting. When you release the button, the temperature will gradually go back to the normal set temperature.

The boost temperature is set in Soldering settings.
