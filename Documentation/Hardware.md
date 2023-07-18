## Notes on the various supported hardware


### TS100

TS100 is a neat soldering iron:
- Can run from 9-25V DC
- Provides a power range that is determined by the input voltage
- Voltages below 12V don't overly work well for any substantial mass
- The default firmware can be found [here](https://www.minidso.com/forum.php?mod=viewthread&tid=892&extra=page%3D1)

![](https://brushlesswhoop.com/images/ts100-og.jpg)

### MHP30

- Accelerometer is the MSA301, this is mounted roughly in the middle of the unit
- USB-PD is using the FUSB302
- The hardware I2C bus on PB6/7 is used for the MSA301 and FUSB302
- The OLED is the same SSD1306 as everything else, but it’s on a bit-banged bus
