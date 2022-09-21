# Hall Effect Sensor

## Sleep Mode Menu

In Sleep mode, the iron automatically lowers the temperature to 150 °C (default). This default was chosen as it is just below the melting point of many solders. A stand-by lower temperature helps reduce the rate of oxidation and prevents damage to iron tips. In general, when not using the iron, unplug it or let it sleep to increase the longevity of replaceable tips. The default sleep temperature can be customized.

Simply moving the iron or pressing any button will wake it back up into soldering mode.

### Optional Hall Effect Feature (Pinecil only):

Inside the [Sleep Menu](https://ralim.github.io/IronOS/Settings/#setting-sleep-temp) is an additional type of sleep setting. Pinecil has an unpopulated footprint (**U14**) for a hall effect sensor, Silicon Labs **Si7210-B-00-IV**. After installing the hall effect sensor (HES), it is possible to auto-trigger Pinecil to enter sleep mode when it enters the stand, and _Zzzz_ will appear (or text in detailed mode). This could be a fun enhancement for any Pinecil and adds a feature typically only found in more expensive high-end irons. The HES is available at many electronic stores for ~$2-$6.

After installing the HES on the PCB, place a magnet on the stand close enough to the sensor to activate one of ten user selectable settings.

- 0=off, 1=1000, 2=750, 3=500, 4=250, 5=150, 6=100, 7=75, 8=50, 9=25 (9 has the highest sensitivity to magnets)
- Setting of 1 might be used if you solder on PCBs with magnets and do not wish Pinecil to auto-sleep constantly. A very strong/large magnet would be required on the stand to activate the sleep mode if you use setting 1.
- Setting of 9 would be useful if you only had a small magnet and are not concerned about Pinecil falsely triggering sleep mode near magnetized items/tools.
- Actively watch the _hall_ number change while you slowly move the magnet around to seek the best locations & whether you have too many or too few magnets. Position the magnet(s) where you have the highest hall number will ensure consistent sleep mode when you place the iron in the stand. This requires some experimenting.
- [See debug menu for how to display the _Hall_ number](https://ralim.github.io/IronOS/DebugMenu/)
- Note that the sensor is physically located near the copper contacts for the tip at the front of the handle. [Reference Schematics U14](https://files.pine64.org/doc/Pinecil/Pinecil_schematic_v1.0a_20201120.pdf).
- Neodymium magnets are recommended. If using small magnets, 2-3 may be required, but too many could also be detrimental.
- Positioning/type/quantity of magnets is important for best results. Sometimes too many magnets breaks the effect by distorting the magnetic field **[as seen in this demo video](https://www.youtube.com/shorts/afkqKwCX00I)**. The video shows magnets at the top of the stand, and the pinecil goes correctly into Zzzz with _only_ those magnets. When more magnets are added at the side, the Pinecil did not go to sleep, which is contrary to the goal. See the PDF below for details on magnetic fields with SI7210-B.
- Orientation of North and South faces of magnets is important to increase reaction of the hall sensor [see data sheet SI7210-B-00-IV](https://www.silabs.com/documents/public/application-notes/an1018-si72xx-sensors.pdf).
