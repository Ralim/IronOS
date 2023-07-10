# GUI Rendering

The GUI aims to be somewhat similar to immediate mode rendering, where the screen is re-rendered each sweep.
This is due to a few aims:

1. Functions should try and contain their state to the context struct (helps keep state usage flatter)
2. Allows external events to change the state
3. Means state can be read/write over BLE or other external control interfaces

## TODO notes

On settings menu exit:

```
      OLED::useSecondaryFramebuffer(true);
      showExitMenuTransition = true;

```
