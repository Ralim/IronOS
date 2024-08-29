## Notes on the various supported hardware

Below are short summaries / notes around the hardware. This is not an in-depth comparison of the features of the units. Please do your own research before purchasing.

Due to descisions out of our control, Miniware no longer provides source-code/schematics/support for any open source firmware on their devices. This does mean that only (TS100/TS80/TS80P) are "open" to any extent. TS80P is pushing that as it was never open at all but just happens to be very close to the TS80. While this generally shouldn't affect the performance of the device, it does mean that their newer products can be slow to be supported or some issues are harder to resolve.

Sequre has so far been supportive of the S60 by providing schematics.

The Pine64 units (Pinecil) are schematics-available (i.e you can download them on the Pine64 Wiki). They are currently the only vendor that has provided financial support of the project. They are also the only vendor that allows contact directly to the engineering teams for hardware issues. This results in generally better support for these devices. It does **not** mean that this firmware is designed around them, but it does help however that they are designed with this firmware in mind as Ralim talks to them. Where possible features are designed to work across all devices but the time for support may vary depending on the hardware and its quirks.


## A quick note on power supplies

For all devices listed **except** the MHP30:

These soldering irons do *NOT* contain DC/DC converters.
This means that your power at the tip is a function of the supplied voltage. Just because the iron "supports" running at a wide range of voltages, you should always use a voltage near the upper limit where possible.
It is highly recommended to use a PD adapter where possible as this allows the iron to _know_ the limitations of your supply.
The marked irons can only turn the tip on and off in software, this means that they can't control the maximum power drawn from the supply. This is why when using PD the iron may select a lower voltage than your power supplies maximum. This is to prevent your power supply failing from over current. For more information about power management underhood, please, [see the related documentation section](https://ralim.github.io/IronOS/Power/).

For the MHP30, it contains a buck DC/DC, which means it can utilise most power supplies fairly well, but you should still aim for highest voltage that is reasonable to use.


### TS100

The TS100 was the first supported soldering iron, and is generally a very capable device.
Its now generally not reccomended to buy new as other devices have all of its features and more, and can often be the same price or cheaper. It's still fully supported though, nothing will be taken away from it.

- can run from 9-25V DC;
- provides a power range that is determined by the input voltage;
- voltages below 12V don't overly work well for any substantial mass;
- the original firmware can be found [here](https://e-design.com.cn/en/NewsDetail/4203645.html)([mirror backup](https://github.com/Ralim/IronOS-Meta/tree/main/Firmware/Miniware)).

![](https://brushlesswhoop.com/images/ts100-og.jpg)

### TS101

The TS101 is the direct replacement of the TS100 with the same tip compatibility.
It adds a spring pressure tip holding mechanism instead of using a screw so tips are easier to swap on the fly (But are held less securely and can pull out depending on the use case). It adds USB-C PD support and the hardware is compatible with 28V EPR power supplies (under both IronOS and official firmware).

It unfortunately uses an STM32 clone MCU with quirks, so performance of the screen isn't as good as it could be but its perfectly usable. The bootloader for programming is the biggest weakness of this device and programming can be a pain. Thankfully IronOS is relatively stable feature wise so you shouldn't need to update the device especially often.

The Miniware bootup logo is burned into their bootloader, so IronOS cant remove this. IronOS can show your own logo when it starts however. There are quirks to loading a logo on this device, so be sure to read the documentation if you are coming from other devices.

### TS80

TS80 is a successor to TS100, it moves to custom smaller tips that perform better at lower wattages. It is optimised for a 9V/2A Quick Charge 3.0 power supply. This is commonly found on older power banks on the USB-A port.
It does **not** support USB-PD and will not work when powered from a USB-C power supply in most cases.

- uses _Quick Charge 3.0_ / _QC3_ capable charger only (18W max);
- doesn't support PD as it is not designed on the hardware level;
- the original firmware can be found [here](https://e-design.com.cn/en/NewsDetail/4203645.html)([mirror backup](https://github.com/Ralim/IronOS-Meta/tree/main/Firmware/Miniware)).

![Image of TS80](https://core-electronics.com.au/media/catalog/product/4/2/4244-01.jpg)


### TS80P

The TS80P is a successor to TS80, and really its what the TS80 should have been. It is nearly identical except it adds USB-PD support for far better compatibility with modern power banks as well as a faster tip removal method.

- supports _Quick Charge 3.0_ (_QC3_: 9V/2A,12V/1.5A 18W max);
- supports _Power Delivery_ (_PD_: 9V/3A & 12V/3A, 30W max)\*\*;
- the original firmware can be found [here](https://e-design.com.cn/en/NewsDetail/4203645.html)([mirror backup](https://github.com/Ralim/IronOS-Meta/tree/main/Firmware/Miniware)).

\*\*: use valid PD device that supports 12V/3A as power source to get full 30W potential, otherwise the iron will fall back to 9V/18W power mode.

![](https://static.eleshop.nl/mage/media/catalog/product/cache/10/image/800x/040ec09b1e35df139433887a97daa66f/s/-/s-l1600_5.jpg)


### MHP30

MHP30 is a **M**ini **H**ot **P**late:

- accelerometer is the MSA301, this is mounted roughly in the middle of the unit;
- USB-PD is using the FUSB302;
- the hardware I2C bus on PB6/7 is used for the MSA301 and FUSB302;
- the OLED is the same SSD1306 as everything else, but it’s on a bit-banged bus;
- the original firmware can be found [here](https://e-design.com.cn/en/NewsDetail/4203645.html)([mirror backup](https://github.com/Ralim/IronOS-Meta/tree/main/Firmware/Miniware)).


### Pinecil

Pincecil:

- first model of soldering iron from PINE64;
- the default firmware can be found [here](https://files.pine64.org/os/Pinecil/Pinecil_firmware_20201115.zip).

![](https://pine64.com/wp-content/uploads/2020/11/pinecil-bb2-04.jpg?v=0446c16e2e66)


