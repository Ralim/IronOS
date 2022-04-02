
# Bouffalo Lab Microcontroller Software Interface Standard

The **NMSIS** is a vendor-independent hardware abstraction layer for micro-controllers that are based on Bl70x Serial processors.

The **NMSIS** defines generic tool interfaces and enables consistent device support.
It provides simple software interfaces to the processor and the peripherals, simplifying software re-use, reducing the learning
curve for micro-controller developers, and reducing the time to market for new devices.

## NMSIS Componments

* **NMSIS CORE**

    All Risc-V Class Processors Standardized API for the Bouffalo Lab processor core and peripherals.

* **NMSIS DSP**

    All Bouffalo Lab Bl70x Class Processors DSP library collection with a lot of functions for various data types:
    fixed-point (fractional q7, q15, q31) and single precision floating-point (32-bit).
    Implementations optimized for the Bl70x processor core which has RISC-V SIMD instruction set.

* **NMSIS NN**

    All Bouffalo Lab Bl70x Class Processors Collection of efficient neural network kernels developed to maximize
    the performance and minimize the memory footprint Bouffalo Lab Bl70x processor core.
 
## Directory Structure

| Directory                  | Content                                                   |
|:-------------------------- |:--------------------------------------------------------- |
| components/nmsis/core      | NMSIS-Core related files                                  |
| components/nmsis/DSP       | NMSIS-DSP related files                                   |
| components/nmsis/NN        | NMSIS-NN related files                                    |
| examples/dsp               | dsp examples                                              |
| examples/nn                | nn examples                                               |
| examples/mfcc              | mfcc compile examples                                     |

## Build Command

In the bl_mcu_sdk root directory, input
`` make APP=BasicMathFunctions_part1 ``

BasicMathFunctions_part1 is project foldor name which is placed in examples/dsp or examples/nn.
User can replace APP=xxx to change build target.


## License

This NMSIS is modified based on open-source project [Nuclei NMSIS](https://github.com/Nuclei-Software/NMSIS) to match Boufflao Lab requirements.
This project transplant NMSIS into bl_mcu_sdk structure for Bouffalo Lab Device Adapting.
NMSIS is licensed under Apache-2.0.
