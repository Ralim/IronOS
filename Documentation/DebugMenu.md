# Debugging Menu

In this firmware there is extra debugging information hidden under an extra menu.
This is accessed by holding the rear (B) button on the iron while it is on the home screen.

This menu is meant to be simple, so it has no fancy GUI animations.

To move through the menu use the front (A) button.
To exit, use the rear (B) button again.

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

This is the handle temperature in C. This is used for cold junction compensation of the tip temp.
This is shown in degrees C x10, so 200 == 20.0C

### Vin 

The input voltage as read by the internal ADC. Can be used to sanity check its being read correctly.

### PCB 

This is slightly miss-named, but preserving the name for now.
This indicates the PCB "version" number, which comes from the TS100 changing the model of accelerometer without warning.
This indicates the accelerometer that is fitted inside the unit.

- 1 = MMA8652
- 2 = LIS2DH12
- 3 = BMA223
- 4 = MSA301
- 5 = SC7A20
- 99 = None detected (running in fallback without movement detection)

### PWR 

This indicates the current power source for the iron.
This may change during power up as the sources are negotiated in turn.

- 0 = DC input (dumb)
- 1 = QC input (We used QC2/3 negotiation for current supply)
- 2 = PD input (We used the PD subsystem to negotiate for the current supply)

### Max 

This indicates the max temp in C that the system estimates it can measure the tip reliably to.
This is dependant on a few factors including the handle temperature so it can move around during use.
