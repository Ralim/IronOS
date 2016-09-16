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

#include "Bios.h"
#include "I2C.h"
#include "MMA8652FC.h"
#include "UI.h"
#include "Oled.h"
#include "CTRL.h"
#include "Hardware.h"
int main(void) {
	RCC_Config(); //setup system clock
	//NVIC_Config(0x4000);
	NVIC_Config(0x0000);
	Init_Timer2(); //init the timers

	GPIO_Config(); //setup all the GPIO pins
	Init_Timer3();
	I2C_Configuration(); //init the i2c bus

	Adc_Init(); //init adc and dma
	if (Get_CtrlStatus() != USB_POWER)
		StartUp_Accelerated(); //start the accelerometer if not in config mode

	System_Init(); //load known safe values
	Init_Oled(); //init the OLED display
	Clear_Screen(); //clear the display buffer to black
	Init_Gtime(); //init the count down timers
	APP_Init(); //pick operating mode via input voltage

	Pid_Init(); //init the pid to starting values
	Set_gKey(NO_KEY); //reset keys to all off
	Start_Watchdog(3000); //start the system watchdog as 3 seconds

	while (1) {
		Clear_Watchdog(); //reset the Watchdog
		if (Get_CtrlStatus() != USB_POWER && LEAVE_WAIT_TIMER== 0) {
			Check_Accelerated(); //update readings from the accelerometer
			LEAVE_WAIT_TIMER = 50;//reset timer so we dont poll accelerometer for another 500ms
		}
		OLed_Display(); //Draw in the Oled display for this mode
		Status_Tran();  //Handle user input and mode changing
	}
}
/******************************** END OF FILE *********************************/
