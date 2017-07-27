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
	setup();/*Setup the system*/
	while (1) {
		Clear_Watchdog(); //reset the Watch dog timer
		ProcessUI();
		DrawUI();
		delayMs(50); //Slow the system down a little bit
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == Bit_RESET) {
			lastMovement = millis();
			//This is a workaround for the line staying low as the user is still moving. (ie sensitivity is too high for their amount of movement)
		}
	}
}
void setup() {
	RCC_Config(); 										//setup system clock
	NVIC_Config(0x4000); //this shifts the NVIC table to be offset, for the usb bootloader's size
	GPIO_Config(); 									//setup all the GPIO pins
	Init_EXTI(); 										//Init the EXTI inputs
	Init_Timer3(); 							//Used for the soldering iron tip
	Adc_Init(); 										//Init adc and DMA
	I2C_Configuration();								//Start the I2C hardware
	GPIO_Init_OLED();						//Init the GPIO ports for the OLED
	restoreSettings();									//Load settings
	StartUp_Accelerometer(systemSettings.sensitivity); //Start the accelerometer
	setupPID(); 										//Init the PID values
	readIronTemp(systemSettings.tempCalibration, 0, 0); //load the default calibration value
	if (systemSettings.OrientationMode == 2)
		Init_Oled(!getOrientation()); 				//Init the OLED display
	else
		Init_Oled(systemSettings.OrientationMode); 			//Init the OLED display

	OLED_DrawString("VER 1.16", 8); 					//Version Number
	delayMs(400);								//Pause to show version number
	showBootLogoIfavailable();
	Start_Watchdog(5000); 		//start the system watch dog as 5 second timeout

}
