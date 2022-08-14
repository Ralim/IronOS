# Debugging Menu

In this firmware there is extra debugging information hidden under an extra menu.
This is accessed by holding the rear (-/B) button on the iron while it is on the home screen.

This menu is meant to be simple, so it has no fancy GUI animations.

To move through the menu, use the front (+/A) button.
To exit, use the rear (-/B) button again.

## Menu items

Items are shown in the menu on a single line, so they use short codes and appear in this order:

### Time

This just shows how many deciseconds the unit has been powered for.

### Move

This is the last timestamp of movement. When the iron is moved, this should update to match the time field (one before in the menu).
This can be used for checking performance of the movement detection code.

### RTip

This is the raw tip reading in μV. This can be used when assessing the calibration routines for example.

### CTip

This is the tip temperature in degrees Celsius.
This can be used with RTip for assessing temperature processing performance.

### CHan

This is the handle temperature in °C. This is used for cold junction compensation of the tip temperature.
This is shown in degrees Celsius x10, so 200 == 20.0 °C

### Vin

The input voltage as read by the internal ADC. Can be used to sanity check its being read correctly.

### ACC

This indicates the accelerometer that is fitted inside the unit.

- MMA8652
- LIS2DH12
- BMA223
- MSA301
- SC7A20
- None detected -> running in fallback without movement detection
- Scanning -> Still searching I2C for one

### PWR

This indicates the current power source for the iron.
This may change during power up as the sources are negotiated in turn.

- **DC** input (dumb)
- **QC** input (We used QC2/3 negotiation for current supply)
- **PD W. VBus** input (PD subsystem is used to negotiate for current supply); and VBus is connected to your input power source
- **PD No VBus** input (PD subsystem is used to negotiate for current supply); and VBus is **NOT** connected to your input power source. If it is Not required or possible to do a special mod of your PCB (i.e. late model V1, some early Green PCB models) then 'PD No VBus' displays on screen, see details and PD Debug [below](/Documentation/DebugMenu.md#pd-debug-menu). Or if you had 'PD Vbus' displayed before and then successfully modified the Pinecil to support 24V by cutting the trace line to Vbus, then you will see 'PD No Vbus' now as a confirmation that the cut worked.

### ID
- This is used by Irons that have an ID and serial number to help check if the iron is authentic. All Pinecil V1 show the same ID number as this is the number programmed into the MCU.
- The new Pinecil V2 released Aug. 2, 2022 now uses BL706, which enables generating a unique ID/Serial number to every iron. This can be used to verify your Pinecil authenticity [here](https://pinecil.pine64.org/).

### Max

This indicates the max temperature in degrees Celsius that the system estimates it can measure the tip reliably to.
This is dependent on a few factors including the handle temperature so it can move around during use.

### HW G

This indicates the high water mark for the stack for the GUI thread. The smaller this number is, the less headroom we have in the stack.
As this is a high-water mater, you should only trust this once you have walked through all GUI options to "hit" the worst one.

### HW M

This indicates the high water mark for the stack for the movement detection thread. The smaller this number is, the less headroom we have in the stack.

### HW P

This indicates the high water mark for the stack for the PID thread. The smaller this number is, the less headroom we have in the stack.

### Hall

This appears if your device is capable of having a magnetic hall effect sensor installed (Pinecil).
This shows the current field strength reading from the sensor. It can be used to check if the sensor is operational, and measure how strong the magnetic field is for diagnostics and optimal placement of magnets on a stand.

# PD Debug menu

On the Pinecil; if the iron is booted up while holding the front button (+); it will show an extra menu for inspecting USB-PD power adapters. Connect to any PD USB power to check Vbus status, even some cell phones with a USB-C port will work if it is PD. It will not show any PD message when Pinecil is powered by DC port, QC, or USB 5V (non-PD).

The menu navigates like the debug menu, where pressing (+) cycles through elements, and (-) will exit the menu.

The first page shows the PD negotiation stage number; which can be used for diagnosing if PD is not working. Once negotiation is complete; the other screens will show the advertised readings for voltage and current of the proposals.

##Below is a method for user modification to convert some early models of Pinecil V1 to safely support 24V on the DC5525 barrel.
Warning (do this at your own risk, read everything in this debug document, and go to the Pine64 chat if you need tips). If you do the cut incorrectly, you could render the Pinecil non-working.

A simple user modification to the PCB on some models of V1 allows it to safely use DC24V by cutting a trace line to the Vbus which held it back to 21V. You can check whether your Pinecil V1 needs the update or can benefit from it by using the PD debug menu. After a few seconds or after PD negotiates (state above 5) it will show [No VBus] if the VBus modification is performed correctly or not needed (i.e., late model V1) or it shows [VBus] if the mod has not been done and there is still a connection to the Vbus.

The mod method is shown in the February 2022 Pine64 community [Updates](https://www.pine64.org/2022/02/15/february-update-chat-with-the-machine/). Early Pinecil V1 models required cutting a trace. Late model V1 made sometime in 2022 came with [No Vbus] already displayed, and no mod required.

The V2 model released Aug. 2, 2022 is an overhauled PCB with all relevant components capable of 28V. V2 requires no mods to support use of 24V DC Barrel port charger.
