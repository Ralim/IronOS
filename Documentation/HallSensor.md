# Hall Effect Sensor

### Sleep Mode Menu

In Sleep mode, the temperature of the iron automatically lowers to a temperature just below the melting point of the solder. This helps reduce the rate of oxidation and damage to the iron tip. In general, when you are not using the iron, turn it off or let it sleep to increase durability of the replaceable tips. The default sleep mode temperature can also be customized.

Simply picking up or moving the iron will wake it back up into soldering mode. You can also press any button and this will also wake the iron up.

####Optional Hall Effect Feature on Pinecil Only:

Pinecil has an unpopulated footprint (U14) for a hall effect sensor (Si7210). Pinecil will sleep after it enters the stand and ZZzz will appear on the screen. After installing the sensor, a magnet is positioned on the stand close enough to the sensor to activate one of 9 sensitivities. 
  - 0 = off, 1 = 1000, 2 = 750, 3 = 500, 4 = 250, 5 = 150, 6 = 100, 7 = 75, 8 = 50, 9 = 25   (9 has the highest sensitivity to magnets)
  - Setting of 1 might be used if you solder on PCBs with magnets and do not wish Pinecil to auto-sleep constantly. And you would also need to use a very strong/large neomydium magnet on the stand to activate the sensor sleep mode.
  - Setting of 9 would be useful if you only had a small magnet and are not concerned about Pinecil falsely triggering sleep mode near magnetized items/tools.
  - Neodymium magnets are recommended. If using small magnets, multiple may be required, but too many can also be detrimental because it deforms the shape of the field away from the sensor.
  - Actively watch the changing hall number (in [debug menu](/Documentation/DebugMenu.md)) while you slowly move the magnet around to seek the best locations & whether you have too many or too few magnets. Positioning the magnet where you have the highest hall number will ensure consistent sleep mode when you place the iron in the stand.
  - The Hall Sensor is physically located near the copper contacts for the tip at the front of the handle. Reference [Schematics U14](https://files.pine64.org/doc/Pinecil/Pinecil_schematic_v1.0a_20201120.pdf).

