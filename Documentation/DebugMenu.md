# Debugging Menu

In this firmware there is extra debugging information hidden under an extra menu.
This is accessed by holding the rear (-/B) button on the iron while it is on the home screen.

This menu is meant to be simple, so it has no fancy GUI animations.

To move through the menu use the front (+/A) button.
To exit, use the rear (-/B) button again.

## Menu items

Items are shown in the menu on a single line, so they use short codes

### HW G

This indicates the High Water mark for the stack for the GUI thread. The smaller this number is, the less headroom we have in the stack.
As this is a Highwater mater, you should only trust this once you have walked through all GUI options to "hit" the worst one.

### HW M

This indicates the High Water mark for the stack for the movement detection thread. The smaller this number is, the less headroom we have in the stack.

### HW P

This indicates the High Water mark for the stack for the PID thread. The smaller this number is, the less headroom we have in the stack.

### Time

This just shows how many deciseconds the unit has been powered for.

### Move

This is the last timestamp of movement. When the iron is moved this should update to match the Time field (one before in the menu).
This can be used for checking performance of the movement dection code.

### RTip

This is the raw tip reading in uV. This can be used when assessing the calibration routines for example.

### CTip

This is the tip temperature in deg C. 
This can be used with RTip for assessing temperature processing performance.

### CHan

This is the handle temperature in C. This is used for cold junction compensation of the tip temperature.
This is shown in degrees C x10, so 200 == 20.0C

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
- **PD W. VBus** input (We used the PD subsystem to negotiate for the current supply); and VBus is connected to your input power source
- **PD No VBus** input (We used the PD subsystem to negotiate for the current supply); and VBus is **NOT** connected to your input power source
- If you successfully modified the Pinecil to support 24V by cutting the trace line to Vbus, then 'PD No VBus' displays on screen.
- Connect to any PD USB power to check Vbus status. It will not show any PD message when Pinecil is powered by DC port, QC, or USB 5V (non PD).

### Max 

This indicates the max temperature in C that the system estimates it can measure the tip reliably to.
This is dependant on a few factors including the handle temperature so it can move around during use.


### Hall

This appears if your device is capable of having a magnetic hall effect sensor installed (Pinecil).
This shows the current field strength reading from the sensor. It can be used to check if the sensor is operational, and measure how strong the magnetic field is for diagnostics and optimal placement of magnets on a stand.
