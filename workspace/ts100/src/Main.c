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
#include "Disk.h"
#include "Bios.h"
#include "usb_lib.h"
#include "I2C.h"
#include "Flash.h"
#include "MMA8652FC.h"
#include "UI.h"
#include "Oled.h"
#include "CTRL.h"
#include "Hardware.h"
int main(void) {
	RCC_Config(); //setup system clock
	NVIC_Config(0x4000);
	Init_Timer2(); //init the timer
	Init_Timer3();
	GPIO_Config();
	USB_Port(DISABLE);
	Delay_Ms(200);
	USB_Port(ENABLE);
	USB_Init();
	I2C_Configuration(); //init the i2c bus

	Adc_Init(); //init adc and dma
	if (Get_CtrlStatus() != CONFIG)
		StartUp_Accelerated();

	System_Init();
	Init_Oled();
	Clear_Screen();
	Init_Gtime();
	APP_Init();

	Disk_BuffInit();
	Config_Analysis(); //read in config from virtual disk
	Pid_Init(); //init the pid to starting values
	Set_gKey(NO_KEY); //reset keys
	Start_Watchdog(3000);

	while (1) {
		Clear_Watchdog(); //reset the Watchdog
		if (Get_CtrlStatus() != CONFIG && LEAVE_WAIT_TIMER== 0) {
			Check_Accelerated(); //update readings from the accelerometer
			LEAVE_WAIT_TIMER = 50;
		}
		OLed_Display(); //Draw in the Oled display
		Status_Tran();  //Handle user input and mode changing
	}
}
/******************************** END OF FILE *********************************/
