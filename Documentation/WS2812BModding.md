# WS2812B RGB Modding (Pinecil V2)

## What is it?

The idea of this mod is to bring the RGB feature of the MHP30 to the Pinecil V2.
Use a transparent shell for a better effect.

Pinecil V2 has a free GPIO_12 accessible through TP10, which is along the screen, cf [Pinecil PCB placement v2.0](https://files.pine64.org/doc/Pinecil/Pinecil_PCB_placement_v2.0_20220608.pdf) page 3. (TP9 (GPIO_14) is also available but hidden below the screen. If you want to use it, change `WS2812B_Pin` in `source/Core/BSP/Pinecilv2/Pins.h`.)

We'll using it to drive a WS2812B and let the color logic already present for the MHP30 do its magic:

- green when temperature is safe (< 55°C)
- pulsing red when heating
- solid red when desired temperature is reached
- orange when cooling down

## Electrical considerations

WS2812B requires a Vdd between 3.5 and 5.3V and Vih (high level of input signal) must be at least 0.7*Vdd.
Pinecil V2 GPIO levels are 3.3V and the 5V rail is actually max 4.6V.
So we can directly power the WS2812B on the 5V rail and command it with the GPIO without need for a level shifter, or for a Zener diode to clamp Vdd.

## How to wire it?

- WS2812B pin 1 (Vdd) is connected to the "5V" rail, e.g. on the C8 capacitor as illustrated [here](https://github.com/Ralim/IronOS/issues/1410#issuecomment-1296064392).
- WS2812B pin 3 (Vss) is connected to the Pinecil GND, e.g. on the U10 pad at the back of the PCB, below R35, as illustrated [here](https://github.com/Ralim/IronOS/issues/1410#issuecomment-1296064392).
- WS2812B pin 4 (Din) is connected to TP10.

You can use e.g. 0.1-mm enameled wire and isolate connections with UV glue to avoid any shortcut.

## How to enable it in the code?

`make firmware-EN model=Pinecilv2 ws2812b_enable=1`
