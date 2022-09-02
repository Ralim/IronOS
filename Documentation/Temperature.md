# Tip temperature measurement

The soldering irons use a modified N-type thermocouple in the tip to measure the tip temperature.
This is constructed for free by using a different type of metal to join one of the rings to the heating coil. This effectively creates a free temperature sensor for very low cost and construction difficulty.

The downsides of this are twofold; one, it is made using non-optimal metals and has a non-constant temperature response; and two, as this uses the same connections as the heating current, you can't measure the temperature while you are heating the tip.



##  How a thermocouple works (brief)

[Thermocouples use a junction of two dissimilar metals](https://www.youtube.com/watch?v=v7NUi88Lxi8) to create a very small amount of power (microvolts). This can then be measured and used with a known transfer function to derive the temperature of the junction.
This has some fairly large limitations, but it also has the benefit of being extremely cheap.

Conventionally a thermocouple is created using two dissimilar metals that join, and then the other ends of these metals are terminated to copper contacts. These copper contacts are also part of the construction of the thermocouple and are referred to as the cold junction.
As there are these extra two joins between the thermocouple wires and the copper; these also have properties of their own in their reactions with temperature.

If the cold junction is held at 0 degrees Celsius, then their effect is considered to be null, and so they can be ignored. However, in the real world the joins to copper are often at room temperature, and as such the measured voltage from the thermocouple must be compensated to remove the influence of these joints. This process is often called cold junction compensation.

Every time in the circuit there is a join between two different metals, then a small thermocouple is created, this means that _every_ soldered connection is also one. 


## How these irons implement the temperature reading

If you analyse one of the open circuit schematics (Pinecil, TS100, TS80) they all use the same approximate formula.
This consists of an op-amp that is connected directly across the heating connections to the tip, and a separate handle temperature sensor.

When the iron is **not** heating the tip, the microcontroller uses the ADC to read the output from the op-amp. This produces a voltage that _should_ be linear to the temperature of (tip-handle). This value is then offset compensated (to remove ADC+op-amp offsets), and then converted into a temperature delta in °C/K. This temperature delta can then be added to the handle temperature to derive the tip temperature in degrees Celsius.

Depending on the construction of the tip, the lookup values used for converting the tip reading in µV into °C/K varies. It is worth noting, however, that TS100 and Pinecil tips are approximately the same as the Hakko T12 tips. (In @Ralim's testing, to within measurement error). This makes sense as the T12 tips are an excellent and cheap design for Miniware to mimic in making the TS100 in the first place.

## Implications of this

### Reading accuracy vs Heating performance tradeoff

Because the tip can only be measured when the unit is not heating, the more often the tip is measured (for finer temperature control) the less time the unit can spend heating up the tip. This means that for fast heat up and fine temperature control the firmware now implements two speeds to the controller loop. During heating up the system runs fewer temperature measurements and instead allows the tip to spend more time burning power. Once the unit is up to temperature, the rate of taking temperature readings is doubled to allow for faster reaction times.

### Tip heat up lag time

As the temperature sensor is a part of the heater coil inside of the tip (or very close by, not entirely certain); the temperature reading is of the _inside_ of the tip, rather than the outside. The outside temperature is the most critical for the user as this is where the solder is actually melting and performing work.

The PID controller in the firmware is tuned to be slightly underdamped and thus more "jumpy" than some people would expect. This is based on the theory that if the inside of the tip is seeing the temperature drop; the outside temperature has dropped more and so we should overcompensate until they equalise.

This is why sometimes the temperature may flick around a little during use but the tip temperature itself is quite stable. The thermal mass of the tip smooths these small amounts out nicely for the user. Though seeing larger jumps on some tips than others _may_ indicate that the tip does not have optimal internal thermal bonding between the heater coil and the tip itself.

The firmware uses the theory that these irons are aimed more to the power users territory than most, so it tries to _not_ hide the actual temperature. Some soldering iron controllers hide the actual measurement once you are within a certain tolerance of this. For example, on a digital Weller unit that Ralim has, if set to 350 °C, it will regulate to within around +/- 3°C but not indicate you are outside of the margin of error until you exceed +/- 5°C. This gives the illusion that it's holding the temperature perfectly when in actuality it's moving around as well.

Given enough time (3-5 seconds) with no external cooling, the inside and outside temperatures of the tip will be equal. When testing the tip temperature accuracy try to allow time for the system to stabilise.

### Complexity of measurement

The firmware in these irons does a *best-effort* of calculating an accurate temperature. As always there is a tradeoff between perfect accuracy and firmware complexity and setup. These irons are built down to a cost; expecting accuracy greater than 1% is not really an option as the voltage reference is only 1% accurate at best. So _all_ measurements are affected by its accuracy. The low-cost chips used in the irons do not come calibrated from the factory so we do not have an internal calibration we can use to try and measure this inaccuracy.

The firmware only accounts for [cold junction compensation](https://www.tegam.com/what-exactly-is-cold-junction-compensation/) and then treats the remaining error as being a constant offset. 
While the error is small, it is actually composed of both a constant offset as well as an offset that is linear to the handle temperature.
This offset that is linear to handle temperature is as of current not modelled into the firmware and is assumed to be constant. This is generally *close enough* as once the unit is in use, the handle temperature is usually within 10 °C as the components inside warm-up from use. This means that this error is "relatively" constant once the unit is being used. 

`However, this can cause odd behaviour when the tip temperature ~= room temperature. It can cause some jumping and movement in the readings when attempting to control the tip to sub 100 °C.`

This is a known tradeoff that is made as the irons intended use case means that it will spend most of its time above 150 °C, at which point these errors are no longer the dominant error sources in the system.
