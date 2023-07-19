# Power sources

Supported by IronOS hardware may use different power sources (chargers/powerbanks/battery packs) with different standards & protocols (QC/PD/etc). This document contains information collected by the community with tested power sources.

This is not ads but first hands-on experience results from real users since some chargers/powerbanks regardless labels on the box may not fully support what's declared!


## QC(3)


### Compatible Devices (QuickCharge for TS80/P)

The following table is the list of compatible device and remarks when powering up the TS80 through it for both stock firmware from MiniDso and IronOS. The list of devices below are primarily taken from [#349](https://github.com/Ralim/ts100/issues/349#issuecomment-449559806)

| Device Name | Stock FW | IronOS FW |
|-------------|:--------:|:---------:|
| Anker PowerCore II Slim 10000 Powerbank | Not Working | Good |
| [Aukey 26.5/30 Ah Powerbank (PB-Y3)](https://www.aukey.com/products/30000mah-power-bank-with-quick-charge-3-0/) | OK\*\* (15sec t/o) | OK\*\* (15sec t/o) |
| Aukey QC3 Charger | Good | Only 5V |
| [Aukey QC3 Charging Station (PA-T11)](https://www.aukey.com/products/6-port-charging-station-with-quick-charge-3-0-pa-t11/) | Good | Good |
| Besiter 20000mah QC3 | Not Working | Only 5V |
| BlitzWolf BW-P5 | Not Working\* | Unknown |
| BlitzWolf BW-PF2 | OK\*\* (10sec t/o) | OK\*\* (10sec t/o) |
| BlitzWolf BW-PL3 | Low Voltage | OK |
| BlitzWolf BW-PL4 | Unknown | Not Working |
| BlitzWolf BW-S6 | Unknown | OK |
| Charmast 20800 mAh QC 3.0 | Low Voltage\* | Good |
| Clas Ohlson Powerbank USB-C 10050 mAh, Clas Ohlson | Unknown | OK\*\* |
| [Cygnett 20,000mAh USB-C](https://www.cygnett.com/products/20-000mah-usb-c-power-bank-in-black)| Not Working | Good |
| [HAME H13D](https://www.amazon.com/dp/B07GWMFW82) 10000mAh PD/QC3.0 Power Bank | OK\*\* (30sec t/o) | OK\*\* (30sec t/o?) |
| HIPER 10000 mAh 18W PD+QC3.0 (MPX10000) | Low Voltage | OK\*\* (30sec t/o) |
| [iMuto Portable Charger 30000mAh](https://www.amazon.com/gp/product/B01MXCMGB8/ref=ppx_yo_dt_b_asin_title_o05_s00) | Low Voltage | Good |
| ISDT BG-8S | Good | Good |
| [iVoler Quick Charge 3.0 USB C 30W 2-Port USB](https://www.amazon.de/dp/B077P8ZZB8/) | Good | Good |
| [imuto X6L Pro 30000 mah](https://www.amazon.com/dp/B01MXCMGB8) | Not Working | Bad |
| Interstep 10000 mAh 18W PD+QC3.0+FCP+AFC (MPX10000) | Good | OK\*\* (30sec t/o) |
| Jackery Black 420 | Low Voltage | Good |
| Kogan Premium 80W 5 Port USB Charger| Low Voltage | Good |
| Nokia AD-18WE | Unknown | OK |
| [Omars 2000mAh USB-C PD+QC3.0 (OMPB20KBUPLT / OMPB20KPLT)](https://www.amazon.com/dp/B07CMLVR6C) | OK\*\* (20sec t/o) | Unknown |
| Polaroid PS100 Powerbank (https://polaroid.com/products/ps100) | Good | Good |
| Xiaomi 10000mAh Mi Power Bank Pro (PLM03ZM) | Good | Unknown |
| Xiaomi 10000mAh Mi Power Bank 2i (PLM09ZM) | Good | Good |
| Xiaomi 20000mAh Mi Power Bank 3 (PLM07ZM) | Unknown | Good Type A, Bad Type C |
| [ZeroLemon ToughJuice](https://www.amazon.com/dp/B01CZR3LT2/) 30000mAh PD/QC2.0 Power Bank | OK\*\* (20sec t/o) | OK\*\* (20sec t/o?) |
| [URUAV XT-60 to USB module](https://www.banggood.com/URUAV-XT-60-to-USB-Charger-Converter-Support-3S-6S-LiPo-Battery-10_5V-32V-Input-3V-20V-Output-45W-Max-Fast-Charging-Adapter-For-RC-Racing-Drone-p-1475876.html) | Unknown | Good |


\* Need further tests on newer firmware

\*\* Most Power Banks shut down if current draw drops below 50mA, assuming that charging is complete and avoiding overcharging. Custom firmware is designed to avoid this until it enters Zzzz mode.


### DIY QC3.0

You may also build your own QC3.0 power source that requires this little [thing](https://www.tindie.com/products/soubitos/qualcomm-qc2-3-diy-8-32vin-36-12vout-3a-max/) and have at least 3S lithium packs or any input voltage from 8 to 32V.

You can also go for an [alternate module](https://www.banggood.com/DC-Buck-Module-12V24V-to-QC3_0-Single-USB-Mobile-Charging-Board-p-1310585.html) which has at least one good review of it.

**DISCLAIMER:** _**We do not hold any responsibility for accidents that happen when building your own QC3.0 power source!!!**_


## PD

The following additional table is the list of devices compatible with hardware which requires Power Delivery support (>= 30W). Devices from the list have been successfully tested & used with TS80P in PD mode. Please, keep in mind that:

- PD can be provided only through usb-c <-> usb-c cable;
- not only a charger but a cable itself should be capable to carry higher wattages.


### Compatible Devices (PowerDelivery for TS80P)

| Device Name | IronOS FW |
|-------------|:---------:|
| Traver Charger QC09 (45W max)\* | OK |
| Xiaomi AD65GEU Mi 65W Fast Charger with GaN Tech (AD65GEU, 65W max) | OK |


Please, DO NOT BUY cheap "fast chargers with QC/PD support" for a few dollars online (i.e., less than ~10$): if you check reviews, then you see that they are phonies - even if you get lucky, you probably get 5V/1A max from them.
