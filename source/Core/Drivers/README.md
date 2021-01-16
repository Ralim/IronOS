# Drivers

Drivers are the classes used to represent physical hardware on the board in a more abstract way, that are more complex than just an IO

* OLED Display
* Accelerometers
* Button handling logic
* Tip thermo response modelling

All drivers should be written with minimal hardware assumptions, and defer hardware related logic to the BSP folder where possible