# Printer demo

## Overview
A printer example created with LVGL. This demo is optmized for 800x480 resolution and among many others it demonstrates custom theme creation, style transitions and animations.

The demo uses some images with 32 bit color depth they require ~1.6 MB flash. With 16 bit color depth ~900KB is required for the images.

![Printer demo with LVGL embedded GUI library](https://github.com/lvgl/lv_examples/blob/master/src/lv_demo_printer/screenshot1.gif?raw=true)

## Run the demo
- In `lv_ex_conf.h` set `LV_USE_DEMO_PRINTER 1`
- After `lv_init()` and initializing the drivers call `lv_demo_printer()`
