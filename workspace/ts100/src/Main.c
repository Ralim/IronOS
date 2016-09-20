/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. **********************
 File Name :      main.c
 Version :        S100 APP Ver 2.11
 Description:
 Author :         Celery
 Data:            2015/07/07
 History:
 2016/8/11	Updates by Ben V. Brown <ralim@ralimtek.com> - Cleanup and english comments
 2015/07/07   ͳһ������
 *******************************************************************************/
#include <string.h>
#include <stdio.h>
#include "APP_Version.h"
#include "Modes.h"
#include "Bios.h"
#include "I2C.h"
#include "MMA8652FC.h"
#include "PID.h"
#include "Oled.h"
#include "Interrupt.h"

int main(void) {
	RCC_Config(); //setup system clock
	//NVIC_Config(0x4000);//this shifts the NVIC table to be offset
	NVIC_Config(0x0000);
	GPIO_Config(); //setup all the GPIO pins
	Init_EXTI();
	//Init_Timer2(); //init the timers

	Init_Timer3();
	I2C_Configuration(); //init the i2c bus

	Adc_Init(); //init adc and dma
	StartUp_Accelerated(); //start the accelerometer

	Init_Oled(); //init the OLED display
	Clear_Screen(); //clear the display buffer to black
	systemSettings.SleepTemp = 1000;
	systemSettings.SleepTime = 1;
	systemSettings.SolderingTemp = 1500;
	readIronTemp(239); //load the default calibration value
	setupPID(); //init the PID values
	//OLED_DrawString("TEST012",7);

	/*for (;;) {

	 OLED_DrawTwoNumber((millis() / 100) % 100, (millis() / 100) % 80);
	 }*/

	//Start_Watchdog(3000); //start the system watchdog as 3 seconds
	while (1) {
		//	Clear_Watchdog(); //reset the Watchdog
		ProcessUI();
		DrawUI();
	}
}
/******************************** END OF FILE *********************************/
