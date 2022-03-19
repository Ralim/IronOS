# Hall Effect Sensor

## Sleep Mode Menu

In Sleep mode, the temperature of the iron automatically lowers to 150 °C (default), which is just below the melting point of the solder. This helps reduce rate of oxidation and damage to the iron tip. In general, when not using the iron, unplug it or let it sleep to increase the longevity of replaceable tips. The default sleep temperature can be customized.

Simply moving the iron will wake it back up into soldering mode. You can also press any button and this will also wake-up the iron.

### Optional Hall Effect Feature (Pinecil only):

Inside the Sleep Menu is an additional type of sleep setting. Pinecil has an unpopulated footprint (U14) for a hall effect sensor (Si7210). It is possible with the hall sensor to trigger Pinecil to go to sleep after it enters the stand, and Zzzz will appear on the screen. After installing the hall sensor, a magnet is positioned on the stand close enough to the sensor to activate one of ten user selectable settings.
  - 0=off, 1=1000, 2=750, 3=500, 4=250, 5=150, 6=100, 7=75, 8=50, 9=25   (9 has the highest sensitivity to magnets)
  - Setting of 1 might be used if you solder on PCBs with magnets and do not wish Pinecil to auto-sleep constantly. And a very strong/large magnet would be required on the stand to activate the sensor sleep mode.
  - Setting of 9 would be useful if you only had a small magnet and are not concerned about Pinecil falsely triggering sleep mode near magnetized items/tools.
  - Neodymium magnets are recommended. If using small magnets, 2-3 may be required, but too many can also be detrimental.
  - Actively watch the hall number change (in [debug menu](/Documentation/DebugMenu.md)) while you slowly move the magnet around to seek the best locations & whether you have too many or too few magnets. Positioning the magnet where you have the highest hall number will ensure consistent sleep mode when you place the iron in the stand. This requires some experimenting.
  - Sensor is physically located near the copper contacts for the tip at the front of the handle. Reference [Schematics U14](https://files.pine64.org/doc/Pinecil/Pinecil_schematic_v1.0a_20201120.pdf).
  - Positioning/type/quantity of magnets is important for best results. Sometimes [too many magnets](https://www.youtube.com/shorts/afkqKwCX00I) breaks the effect by distorting the magnetic field.
  - Orientation of North and South faces of magnets is important to increase reaction of the hall sensor [SI7210-B-00-IV](https://www.silabs.com/documents/public/application-notes/an1018-si72xx-sensors.pdf).

