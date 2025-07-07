# Debugging PD

When using many of these soldering irons, the recommended power source is to use a USB-PD power supply.

Occasionally, issues are run into where the iron reboots or appears to not boot when connected to this supply.

There are generally a few different reasons for this to occur, the first is of course a bug or incompatibility in the IronOS PD-stack / firmware, but there are also power adapters that either have issues or try to be _smart_ to the detriment of compatibility.

It also helps to remember that driving a soldering iron is not like a normal load that these power supplies are designed for. Normally a laptop or phone will gently ramp the power draw up and down. Where as the soldering iron will rapidly go from 0 to full power, and then back to 0 again. This can cause issues with some power supplies tripping out.

In general, a normal, boring 60-100W PD supply is recommended. Watch out for adapters with multiple ports that are used by marketing to advertise a higher number. It's somewhat common to see 65W adapters being pushed that have two ports, one of which is 45W and one that is 20W. These cannot support 65W output on one typically.

Smarter chargers that try to implement every known protocol can come with quirks. Often slight shortcuts are taken in the PD implementation that can cause hard to debug issues.


## If the unit doesn't power up at all

This can be the most frustrating one to diagnose.

First, test the device powers up when powered by a USB-A -> USB-C cable. Or a DC power supply. This can rule out other issues that cause the device to appear off (bad flashing).

### No power
If your device won't power up on any other supply type, look into if you can boot into the bootloader. This is usually done by holding down a button while connecting it to a computer and then checking if it's detected.

If the device shows up to a computer, but doesn't operate when powered up normally, the two most likely causes are a bad flash/firmware OR a non-functioning display.

Testing alternative firmware builds or trying to heat the unit (pressing the front button) can be ways to test this.

### Powers up on other supplies

If the device powers up on other supplies, but not on the USB-PD supply, it could be a problem with the USB-PD supply itself. Try using a different USB-PD supply to see if the issue persists.

If the unit does not power on any PD supplies it could be damage to the PD PHY or the USB connector. USB-PD uses the CC pins on the connector, which are not used for normal data so a USB-A adapter for example doesn't use these at all.

## If the unit powers up but keeps rebooting

There are two causes of this:
1. If the reboot occurs when the unit starts to heat up, then it is the power supply being unable to supply the power requested.
2. The unit reboots frequently even without any buttons being pressed.

If this is the issue that you are seeing, then the problem is that something during the PD initialisation is failing.

The _best_ way to resolve this is to be able to capture the USB PD traffic. This is the only way to know what is **really** going on and why the two devices can't negotiate.

To capture PD traffic requires a device that can capture this data. A logic analyser can be used on the CC pins, though note that the signalling voltage is < 3.3V so it will require a logic analyser that can handle this or buffering.

Alternatively, a lot of the higher-end USB power meter units can capture the packets. It doesn't matter if it only shows these on screen or if it can save these out to a file (ideally a file though).

**Without a traffic capture, all debugging is guessing**

On firmwares 2.23+ there is a toggle in advanced settings to change the PD mode. This will adjust how the firmware negotiates with the PD supply slightly. This can enable/disable the PPS and EPR modes (dynamic voltage negotiation).

PPS is known to be incorrectly implemented on some supplies, so turning off these features can improve compatibility.

If the device is _sometimes_ stable, you can on Pinecil devices boot while holding the front button to enter the PD debug menu. This will show what voltages & power levels are being advertised by the device. This can be used to cross-check with what is printed on the adapter. Take into consideration that non e-marked cables will be limited to 3A and that EPR requires specifically marked cables.

If you take the tip out of the iron, it will result in most devices not negotiating a PD profile (the irons wait to know what kind of tip is installed). This can be used to stop the failing negotiations in some situations to allow viewing this menu.


Before filing a support request, please try testing other power adapters & cables to try and narrow down the possibilities of the issue being a one-off.
If you have the capability to capture the PD traffic, that makes the problem exponentially easier to rectify.
