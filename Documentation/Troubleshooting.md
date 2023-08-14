# Troubleshooting

If your device is not operating as expected; and you are within the manufacturer support window, please first contact your manufacturer and RMA / warranty your device.

If your iron is not working as expected, [the Debug menu](https://ralim.github.io/IronOS/DebugMenu/) exposes internal measurements to help you narrow down the root cause of the issue.

Alongside all of these, issues with the soldering of the main MCU could cause all of these as well; and should always be checked.

The tip is important for the operation of your iron. T100 and Pinecil tips are around 8 ohms, and TS80(P) tips are around 4.5 ohms.

You are welcome to open discussions about issues as well, or if you bought your Pinecil from an official store; use the [Pinecil community chat](https://wiki.pine64.org/wiki/Pinecil#Community_links) for support.
But it is helpful to do some basic diagnostics first just in case the issue is easily fixed.

The **VAST** majority of issues are poor soldering or cold solder joints.
If you can open up your iron, give it a good look at all the connection points, and use another iron to reflow any suspicious ones, this can fix most issues.

## Tip Shorted warning

If you are powering up a device that supports tip resistance detection (TS101 and Pinecilv2 as of present), the firmware checks the readings of the raw tip resistance and sorts these into three "bins". `8 ohm tips`, `6.2 ohm tips` and `tip-shorted`. The tip resistance is used when negotiating USB-PD and in thermal calculations.
The `tip-shorted` option is selected if your tip is measured to be abnormally small. This could indicate a failed driver mosfet or a failed tip.

When this warning is shown; heating will be disabled to protect from damage. As trying to heat a shorted tip can damage the iron itself.

It is best to take out your tip and manually measure and verify the tip's resistance. It should be 6-8 ohms (depending on tip type). When measuring resistances this small some multimeters can struggle. If you have access to a current limited bench power supply, you can try doing a 4 wire measurement by measuring the voltage drop on the tip while applying a known current. `(R=V/I)`.

If the tip measures correctly you may have a damaged driver mosfet; it would be ideal to open your iron and test the mosfet is operating correctly.
If after both of these checks everything looks as expected, feel free to open a discussion on IronOS to talk about the issue (Or for Pinecil the community chat can be a much faster response).

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

If you have a poor connection or a floating pin, you can end up with a state where the screen works _sometimes_ and then freezes or only works on some power cycles. It might work on very old versions of IronOS but not the newest ones. You could try to reflow the pins for the OLED. On 96x16 screens, carefully peel it back from the adhesive and reflow the solder on the pins.

As the OLED runs on an I2C bus, there are pull up resistors on the SDA and SCL pins. It is worth checking these as well, while they don't often fail, issues with these can cause _weird_ display issues.

If after all of the checks OLED is still blank, or screen works but pixels are barely visible, although soldering iron itself is working (i.e., you can safely check that it's turning on, heating up & melting solder successfully), then it means that _most likely_ OLED is dead. But it can be relatively easily replaced. Models like `TS100`, `TS80`, and `TS80P` share the same OLED screen which can be bought online and used for replacement. To do so:

- find & buy at electronics shop [of your choice] display with the following spec line:  
```OLED 0.69 inch / 14 pins / 96 x 16 pixels / **9616TSWC** / I2C IIC```

- disassemble your soldering iron;
- desolder old OLED and solder back new one;
- assemble your soldering iron back.

There are a few youtube videos how to do it like [this one for `TS100`](https://www.youtube.com/watch?v=HlWAY0oYPFI).

Unfortunately, this is a well-known issue of screens with OLED technology: sooner or later the brightness is starting to _"fade out"_ until complete off. Usually common recommendations to prolong its lifetime are: reduce brightness & reduce too often updates (i.e., disable animations). But your results may vary since there were reports when users couldn't see anything after turning on soldering irons which were just laying in a box for a few months after buying. And there are users with first `TS100` models not having any issues with display at all.

## Tip heats when not in heating mode

⚠️ DISCONNECT YOUR TIP ⚠️

Most likely you have either a blown MOSFET or shorted pin.
Check the MOSFET and also its driver transistor.
The firmware will not enable the tip until you are in soldering mode.

## Accelerometer not detected

Your Iron may have a new accelerometer that is not supported yet (happens every year or so) OR there is a soldering issue with the accelerometer (reflow/resolder).
