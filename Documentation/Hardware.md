## Notes on the various supported hardware

### MHP30

- Accelerometer is the MSA301, this is mounted roughly in the middle of the unit
- USB-PD is using the FUSB302
- The hardware I2C bus on PB6/7 is used for the MSA301 and FUSB302
- The OLED is the same SSD1306 as everything else, but it’s on a bit-banged bus
