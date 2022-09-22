# Troubleshooting

If your device is not operating as expected; and you are within the manufacturer support window, please first contact your manufacturer and RMA / warranty your device.

If your iron is not working as expected, [the Debug menu](https://ralim.github.io/IronOS/DebugMenu/) exposes internal measurements to help you narrow down the root cause of the issue.

Alongside all of these, issues with the soldering of the main MCU could cause all of these as well; and should always be checked.

The tip is important for the operation of your iron. T100 and Pinecil tips are around 8 ohms, and TS80(P) tips are around 4.5 ohms.

You are welcome to open discussions about issues as well, or if you bought your Pinecil from an official store; use the [Pinecil community chat](https://wiki.pine64.org/wiki/Pinecil#Community_links) for support.
But it is helpful to do some basic diagnostics first just in case the issue is easily fixed.

The **VAST** majority of issues are poor soldering or cold solder joints.
If you can open up your iron, give it a good look at all the connection points, and use another iron to reflow any suspicious ones, this can fix most issues.

## High tip temp reading when the tip is cool

If you are finding the tip is reading high; the first fields to check in the Debug menu are `RTip` and `CHan`.

- `RTip` is the raw tip reading in μV; at cool this should be around 700-1000 for larger tips and ~1500 for smaller tips (TS80's)
- `CHan` is the temperature of the temperature sensor on the PCB in degrees Celsius \* 10. So 29 °C ambient should read as 290

### RTip is out of spec

`RTip` will over-read on bad contacts or no tip inserted.

If `RTip` is overreading, you may have one of the following:

- Partially stuck on main MOSFET
- Slow reacting main MOSFET driver transistor
- Damaged Op-Amp
- Poor soldering on the Op-Amp circuitry
- No tip inserted or tip that is not connecting correctly

If `RTip` is under-reading you most likely have issues with the Op-Amp or the tip. The signal should be pulled high by hardware (reading hot), so this often means the MCU is not reading the signal correctly. Check MCU soldering.

### CHan is out of spec

CHan reading comes directly from the cold junction compensation temperature sensor.
This is usually a TMP36 (Pinecil V1), or an NTC thermistor (MHP30, TS80P, Pinecil V2).

If `CHan` is reading low:

- Check the connection from the MCU to the handle temperature sensor.
- Check the power pin connection on the TMP36
- Check pullup resistor on the NTC thermistor
- Check no bridged pins or weak shorts on the signal to nearby pins on MCU or temperature sensor
- Reflow/resolder the aforementioned components

If `CHan` is reading higher

- Check ground connections on the sensors
- Check no bridged pins or weak shorts on the signal to nearby pins on MCU or temperature sensor
- Reflow/resolder the aforementioned components

## No display OR dots on the display

If when you power up your iron you get no display, the first test is to (carefully) attempt to heat the tip.
Press the front button (`+/A`) on your device and check if the tip heats up.
If the tip does not heat up, it is worth trying to reflash the firmware first in case it is corrupted.

The main failure mode of the OLED display module is usually poor soldering on the OLED display cable to the main PCB.
As this is soldered by hand generally, it's the most prone to failures.

If you have a poor connection or a floating pin, you can end up with a state where the screen works _sometimes_ and then freezes or only works on some power cycles. It might work on very old versions of IronOS but not the newest ones. You could try to reflow the pins for the OLED. On 96x16 screens, carefully peel it back from the adhesive and reflow the solder on the pins. If needed, replacement Oled screens are common and low cost.

As the OLED runs on an I2C bus, there are pull up resistors on the SDA and SCL pins. It is worth checking these as well, while they don't often fail, issues with these can cause _weird_ display issues.

## Tip heats when not in heating mode

⚠️ DISCONNECT YOUR TIP ⚠️

Most likely you have either a blown MOSFET or shorted pin.
Check the MOSFET and also its driver transistor.
The firmware will not enable the tip until you are in soldering mode.

## Accelerometer not detected

Your Iron may have a new accelerometer that is not supported yet (happens every year or so) OR there is a soldering issue with the accelerometer (reflow/resolder).
