## Notes on the various supported hardware


### TS100

TS100\* is a neat soldering iron:

- can run from 9-25V DC;
- provides a power range that is determined by the input voltage;
- voltages below 12V don't overly work well for any substantial mass;
- the original firmware can be found [here](https://e-design.com.cn/en/NewsDetail/4203645.html).

![](https://brushlesswhoop.com/images/ts100-og.jpg)


### TS80

TS80\* is a successor to TS100:

- uses _Quick Charge 3.0_ / _QC3_ capable charger only (18W max);
- doesn't support PD as it is not designed on the hardware level;
- the original firmware can be found [here](https://e-design.com.cn/en/NewsDetail/4203645.html).

![](https://core-electronics.com.au/media/catalog/product/4/2/4244-01.jpg)


### TS80P

TS80P\* is a successor to TS80:

- supports _Quick Charge 3.0_ (_QC3_: 9V/3A, 18W max);
- supports _Power Delivery_ (_PD_: 9V/3A & 12V/3A, 30W max)\*\*;
- the original firmware can be found [here](https://e-design.com.cn/en/NewsDetail/4203645.html).

\*\*: use valid PD device that supports 12V/3A as power source to get full 30W potential, otherwise the iron will fall back to 9V/18W power mode.

![](https://static.eleshop.nl/mage/media/catalog/product/cache/10/image/800x/040ec09b1e35df139433887a97daa66f/s/-/s-l1600_5.jpg)


### MHP30

MHP30 is a **M**ini **H**ot **P**late:

- accelerometer is the MSA301, this is mounted roughly in the middle of the unit;
- USB-PD is using the FUSB302;
- the hardware I2C bus on PB6/7 is used for the MSA301 and FUSB302;
- the OLED is the same SSD1306 as everything else, but it’s on a bit-banged bus;
- the original firmware can be found [here](https://e-design.com.cn/en/NewsDetail/4203645.html).


### Pinecil

Pincecil\*:

- first model of soldering iron from PINE64;
- the default firmware can be found [here](https://files.pine64.org/os/Pinecil/Pinecil_firmware_20201115.zip).

![](https://pine64.com/wp-content/uploads/2020/11/pinecil-bb2-04.jpg?v=0446c16e2e66)


\*: Please note: these soldering irons do *NOT* contain DC/DC converters. This means that your power at the tip is a function of the supplied voltage. Just because the iron "supports" running at a wide range of voltages, you should always use a voltage near the upper limit where possible. It is highly recommended to use a PD adapter where possible as this allows the iron to _know_ the limitations of your supply. The marked irons can only turn the tip on and off in software, this means that they can't control the maximum power drawn from the supply. This is why when using PD the iron may select a lower voltage than your power supplies maximum. This is to prevent your power supply failing from over current. For more information about power management underhood, please, [see the related documentation section](https://ralim.github.io/IronOS/Power/).


