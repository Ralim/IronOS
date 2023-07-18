## Notes on the various supported hardware


### TS100

TS100 is a neat soldering iron:
- can run from 9-25V DC;
- provides a power range that is determined by the input voltage;
- voltages below 12V don't overly work well for any substantial mass;
- the default firmware can be found [here](https://www.minidso.com/forum.php?mod=viewthread&tid=892&extra=page%3D1).

![](https://brushlesswhoop.com/images/ts100-og.jpg)


### TS80

TS80 is a successor to TS100:
- uses _Quick Charge 3.0_ / _QC3_ capable charger only;
- doesn't support PD as it is not designed on the hardware level;
- the default firmware can be found [here](https://www.minidso.com/forum.php?mod=viewthread&tid=3208&extra=page%3D1).

![](https://core-electronics.com.au/media/catalog/product/4/2/4244-01.jpg)


### MHP30

MHP30 is a **M**ini **H**ot **P**late:
- accelerometer is the MSA301, this is mounted roughly in the middle of the unit;
- USB-PD is using the FUSB302;
- the hardware I2C bus on PB6/7 is used for the MSA301 and FUSB302;
- the OLED is the same SSD1306 as everything else, but it’s on a bit-banged bus.


