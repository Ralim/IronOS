/*
 * Created by Ben V. Brown
 */

#include "Modes.h"
#include "Bios.h"
#include "MMA8652FC.h"
#include "PID.h"
#include "Oled.h"
#include "Settings.h"
#include "I2C.h"
void setup();

int main(void) {
	setup();
	while (1) {
		Clear_Watchdog(); //reset the Watchdog
		ProcessUI();
		DrawUI();
		delayMs(50);
	}
}
void setup()
{
	RCC_Config(); 							//setup system clock
		NVIC_Config(0x4000); 				//this shifts the NVIC table to be offset, for the usb bootloader's size
		GPIO_Config(); 						//setup all the GPIO pins
		Init_EXTI(); 						//init the EXTI inputs
		Init_Timer3(); 						//Used for the soldering iron tip
		Adc_Init(); 						//init adc and dma
		I2C_Configuration();				//Start the I2C hardware
		GPIO_Init_OLED();					//Init the GPIO ports for the OLED
		StartUp_Accelerometer(); 			//start the accelerometer
		Init_Oled(); 						//init the OLED display
		Clear_Screen(); 					//clear the display buffer to black
		setupPID(); 						//init the PID values
		readIronTemp(239, 0); 				//load the default calibration value
		restoreSettings();					//Load settings

		Start_Watchdog(1000); //start the system watchdog as 1 seconds timeout
}
