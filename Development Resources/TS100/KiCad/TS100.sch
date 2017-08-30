EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:stm32
LIBS:mma8652fc
LIBS:switches
LIBS:TS100-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 2
Title "TS100 Soldering Iron Schematic"
Date "2017-05-18"
Rev "2.46"
Comp ""
Comment1 ""
Comment2 ""
Comment3 "Converted by Ben V. Brown"
Comment4 "KiCad port of published schematic"
$EndDescr
$Sheet
S 1450 1000 1450 1850
U 591D5966
F0 "STM32 & Accel Sub Board" 60
F1 "MCU_SubBoard.sch" 60
F2 "SWDIO" I R 2900 1100 60 
F3 "SWCLK" I R 2900 1200 60 
F4 "USB_D-" I R 2900 1350 60 
F5 "USB_D+" I R 2900 1450 60 
F6 "K1" I R 2900 1650 60 
F7 "nCR" I R 2900 2150 60 
F8 "TMP36" I R 2900 2250 60 
F9 "K2" I R 2900 1750 60 
F10 "SCL" I R 2900 2550 60 
F11 "SDA" I R 2900 2650 60 
F12 "Po" I R 2900 2050 60 
F13 "Vb" I R 2900 1950 60 
$EndSheet
$Comp
L BARREL_JACK J101
U 1 1 591D8B75
P 1600 3600
F 0 "J101" H 1600 3795 50  0000 C CNN
F 1 "BARREL_JACK" H 1600 3445 50  0000 C CNN
F 2 "" H 1600 3600 50  0001 C CNN
F 3 "" H 1600 3600 50  0001 C CNN
	1    1600 3600
	1    0    0    -1  
$EndComp
Text Label 1900 3500 0    60   ~ 0
VIN
$Comp
L GND #PWR103
U 1 1 591D8F24
P 2000 3800
F 0 "#PWR103" H 2000 3550 50  0001 C CNN
F 1 "GND" H 2000 3650 50  0000 C CNN
F 2 "" H 2000 3800 50  0001 C CNN
F 3 "" H 2000 3800 50  0001 C CNN
	1    2000 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 3600 2000 3600
Wire Wire Line
	2000 3600 2000 3800
Wire Wire Line
	1900 3700 2000 3700
Connection ~ 2000 3700
$Comp
L SW_Push SW1
U 1 1 591D9BC6
P 1400 4550
F 0 "SW1" H 1450 4650 50  0000 L CNN
F 1 "SW_Push" H 1400 4490 50  0000 C CNN
F 2 "" H 1400 4750 50  0001 C CNN
F 3 "" H 1400 4750 50  0001 C CNN
F 4 "-" H 1400 4550 60  0001 C CNN "bom_partno"
	1    1400 4550
	0    1    1    0   
$EndComp
$Comp
L SW_Push SW101
U 1 1 591DA371
P 1700 4550
F 0 "SW101" H 1750 4650 50  0000 L CNN
F 1 "SW_Push" H 1700 4490 50  0000 C CNN
F 2 "" H 1700 4750 50  0001 C CNN
F 3 "" H 1700 4750 50  0001 C CNN
	1    1700 4550
	0    1    1    0   
$EndComp
$Comp
L GND #PWR102
U 1 1 591DA3DA
P 1700 4750
F 0 "#PWR102" H 1700 4500 50  0001 C CNN
F 1 "GND" H 1700 4600 50  0000 C CNN
F 2 "" H 1700 4750 50  0001 C CNN
F 3 "" H 1700 4750 50  0001 C CNN
	1    1700 4750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR101
U 1 1 591DA436
P 1400 4750
F 0 "#PWR101" H 1400 4500 50  0001 C CNN
F 1 "GND" H 1400 4600 50  0000 C CNN
F 2 "" H 1400 4750 50  0001 C CNN
F 3 "" H 1400 4750 50  0001 C CNN
	1    1400 4750
	1    0    0    -1  
$EndComp
Text Label 1400 4350 0    60   ~ 0
K1
Text Label 1700 4350 0    60   ~ 0
K2
Text Label 2900 1750 0    60   ~ 0
K2
Text Label 2900 1650 0    60   ~ 0
K1
$EndSCHEMATC
