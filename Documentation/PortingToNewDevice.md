# Requesting support for a new device

IronOS is largely designed to run on devices that are using _fairly_ modern microcontrollers at their core. Generally this means an ARM Cortex or RISC-V processor.
At this point in time it is not planned to support 8051 or similar cored devices. This is largely due to the reliance on FreeRTOS at the moment.

When requesting a port for a new device, please try and find out if the hardware meets the below requirements.

The feature list's below are organised into three categories; Hard requirements that as of current must be met, soft requirements that _should_ be met for full featured performance and the final category of planned _but not yet implemented_ features; which can be implemented but can result in delays as these are not yet implemented.

Aside from the below, keep in mind IronOS is really designed for soldering irons. This has expanded out into hot-plates as they are exceptionally similar devices.

## Hard requirements

1. Supported processor (Arm Cortex or RISC-V). (Though generally anything that has an existing FreeRTOS port is possible).
2. 64K of flash or larger (See note A)
3. 16K of ram or larger
4. Device has one or more heating elements that can be controlled by a main temperature sensor
5. If the main temperature sensor is a thermocouple, a reference temperature sensor for cold junction compensation must exist and be close to the sensor contacts
6. Means of the user updating the device without opening
7. Known pinmap for the microcontroller. (see note B)

## Soft requirements

1. USB-PD is strongly preferred over Quick Charge; Quick Charge only devices are considered legacy and will likely not be prioritiesd.
2. Open source or at the least schematics available is **strongly** preferred and will prioritise the device.
3. Likewise friendly vendors will help dramatically with support, due to both questions and also appearances to help the community.
4. Hardware PWM wired up to the tip control is nice to have but not essential
5. Very strong preference against devices that use the endless sea of STM32 clones.

## Planned features

These features are planned for eventual support, but will likely not be done until devices need them.

- Colour screens
- More than 2 buttons for input, or encoder inputs
- WiFi/Zigbee/ any other networking

## Notes

### Note A - Flash storage space

64KB is generally the minimum recommended size for the hardware to have.
Larger is _definitely_ preferred as it enables more features or the multi-pack language firmwares.
Keep in mind that on some devices we loose space to a USB DFU bootloader (Older STM32F1's) so the firmware _can_ work with less. But it can come at the cost of features.
128KB or larger is **great**.
For devices that have BLE or WiFi or other features, often code requirements are significantly larger. These are considered non essential features so will be ignored if we run into size issues.

### Note B - Pinmap for the microcontroller

In order to be able to write the interfacing code to communicate with the hardware, we need to know what pins on the microcontroller go to what hardware.
It is also loosely required to have an understanding of the rest of the device, we do not need details on a lot of the boring aspects,but if for example a USB-PD interface IC is used we would want to know which one.

## Example request for adding a new device

Device Name:
Device Type:
Approximate Price:
Example purchase locations:

### Hardware details

Microcontroller version: `STM32F103C8Tx`
Flash size (If external to the MCU):`N/A`
Microcontroller Pinout: <!-- Either link to manufacturer information, a forum documenting this or a discussion where the pinout has been roughly figured out already-->
Device type: <!-- Soldering Iron/Hot Plate/ Reflow oven etc-->
Device meets hard requirements list []
Device meets soft requirements list []

Device features USB-PD []
Device features USB-QC []
Device features DC Input []
Device features BLE []
