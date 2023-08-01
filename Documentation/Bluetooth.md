# Bluetooth Low Energy

The Pinecilv2 has hardware support for Bluetooth Low Energy (BLE). This protocol allows reading and writing of parameters to the Pinecil during runtime.

The BLE interface advertises three services, these provide access to live telemetry as well as the ability to read/write settings.
These are outlined in more detail below.

Pinecil devices advertise themselves on BLE as `Pinecil-XXXXXXX`.
They also include the UUID `9eae1000-9d0d-48c5-AA55-33e27f9bc533` in the advertisement packet to allow for filtering.

Unless otherwise noted, all data is sent and received as Little-Endian.

As of the time of writing this, notifications are not fully implemented so data will need to be polled. Notification/Indication support will come when there is time to implement it.

## Using the BLE Interface

It is advised to follow the below points when first implementing a BLE integration. Of course once the integration is working feel free to deviate from these. These are just _suggested_ ideas to help kickstart.

1. When filtering for devices, its preferable to filter by the UUID `9eae1000-9d0d-48c5-AA55-33e27f9bc533`, rather than by the device name if possible.
2. Upon first collection check if the three expected services exist; if they don't the user may have selected an incorrect device.
3. It's best to read the live bulk endpoint over the live service when its easy to do so (one read vs ~15).
   1. However if you are just updating one or two line items it may be more efficient to just read these on the live service.
   2. Feel free to test both and decide.
4. When reading settings from the device; the association of number <-> setting is fixed, but you may see settings you don't yet know about, make sure you can handle these.
5. You probably don't want to show unknown setting's to the user though.
6. Read the device firmware revision and ensure you can decode it. If BLE is revised it may be essential for handling versions cleanly.
7. It's advisable to keep an eye on the IronOS repository or at least setup the Github watch for release notifications.
   1. Future releases may revise some BLE aspects or add new settings for example.

## Services

Below is a description of each service. Note that the exact settings are not listed for brevity; its best to refer to [the uuid lists](https://github.com/Ralim/IronOS/blob/dev/source/Core/BSP/Pinecilv2/ble_characteristics.h) and the [handlers](https://github.com/Ralim/IronOS/blob/dev/source/Core/BSP/Pinecilv2/ble_handlers.cpp) alongside this.

### Live

`UUID: d85ef000-168e-4a71-AA55-33e27f9bc533`

The live services has one characteristic per reading. The readings (in order) are:
When implementing these; the ones that are not obvious are generally found in the debugging menu. Values are encoded as an unsigned 32 bit number for all results.

1. Live temperature (In C)
2. Live set point
3. DC input voltage
4. Handle temperature (In C)
5. Power level
6. Power source
7. Tip resistance
8. uptime
9. Time of last movement
10. Maximum temperature settable
11. Raw tip reading
12. Hall sensor
13. Operating mode
14. Estimated wattage

### Settings

`UUID: f6d80000-5a10-4eba-AA55-33e27f9bc533`

The settings service has two special entries; for saving and resetting settings.
Otherwise all settings are enumerated using uuid's of the format : `f6d7ZZZZ-5a10-4eba-AA55-33e27f9bc533))` where `ZZZZ` is the setting number as matched from [Settings.h](https://github.com/Ralim/IronOS/blob/dev/source/Core/Inc/Settings.h#L16).

All data is read and written in fixed unsigned 16 bit numbers.

#### Settings save

To save the settings write a `0x0001` to `f6d7FFFF-5a10-4eba-AA55-33e27f9bc533`.
Its advised to not save settings on each change but instead to give the user a save button _or_ save after a timeout. This is just to reduce write cycles on the internal flash.

#### Settings reset

To reset all settings to defaults; write a `0x0001` to  `f6d7FFFE-5a10-4eba-AA55-33e27f9bc533`.
This will reset settings immediately.

### Bulk

`UUID: 9eae1000-9d0d-48c5-AA55-33e27f9bc533`

The bulk endpoint is where extra data is located with varying read sizes.

#### Live data

The bulk live data endpoint provides all of the data provided in the live endpoint, as one large single-read binary blob. This is designed for applications that are showing large amounts of data as this is more efficient for reading.

#### Accelerometer Name

_Not yet implemented_

#### Build ID

This encodes the current build id to allow for display as well of handling incase the BLE format changes

#### Device Serial Number

This is generally the device CPU serial number. For most devices this can be used as an id. On PinecilV2 its the MAC address.

#### Device Unique ID

This is only relevant on the PinecilV2. This is a random ID that is burned in at the factory. This is used by the online authenticity checker tool.
