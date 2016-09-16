/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. **********************
 File Name :      CTRL.c
 Version :        S100 APP Ver 2.11
 Description:
 Author :         Celery
 Data:            2015/08/03
 History:
 2015/07/07   ͳһ������
 2015/08/03   �Ż��ƶ��ж�
 *******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "CTRL.h"
#include "Bios.h"
#include "UI.h"
#include "Hardware.h"
#include "S100V0_1.h"
#include "MMA8652FC.h"

#define HEATINGCYCLE  30
/******************************************************************************/
DEVICE_INFO_SYS device_info;
/******************************************************************************/

u8 gCtrl_status = 1;
//^ System current status / operating mode.
u16 gHt_flag = 0;
vs16 gTemp_data = 250; //
s16 gPrev_temp = 250; //
u8 gIs_restartkey = 0; //
u8 gPre_status = 1; //

const DEVICE_INFO_SYS info_def = { "2.13",     //Ver
		1000,       //T_Standby;    // 200C=1800  2520
		1000,       // T_Work;      // 350C=3362,
		100,        //T_Step;
		3 * 60 * 100,   //Wait_Time;    //3*60*100   3  minutes
		6 * 60 * 100    // Idle_Time;   //6*60*100  6 minutes
};
struct _pid {
	s16 settemp;        //Current ideal setpoint for the temp
	s16 actualtemp;     //Actual current temp of the tip
	s16 err;            //Error term
	s16 err_last;       //last error term
	u32 ht_time;        //
	u16 kp, ki, kd;       //Constants for the PID Controller
	s32 integral;       //
} pid;

/*******************************************************************************
 Function: Get_Ctrl_Status
 Description: Returns the current operating Mode
 Input:Void
 Output:Current System Status
 *******************************************************************************/
u8 Get_CtrlStatus(void) {
	return gCtrl_status;
}
/*******************************************************************************
 Function: Set_CtrlStatus
 Description: Set the current operating Mode
 Input:status uint8_t
 Output:Void
 *******************************************************************************/
void Set_CtrlStatus(u8 status) {
	gCtrl_status = status;
}
/*******************************************************************************
 Function: Set_PrevTemp
 Description:Set the previous temp record for the PID
 Input:Previous Temp (int16_t)
 Output:Void
 *******************************************************************************/
void Set_PrevTemp(s16 temp) {
	gPrev_temp = temp;
}

/*******************************************************************************
 Function: Get_HtFlag
 Description:
 Input:Void
 Output:Void

 *******************************************************************************/
u16 Get_HtFlag(void) {
	return gHt_flag;
}
/*******************************************************************************
 Function:Get_TempVal
 Description:
 Input:Void
 Output:Void
 *******************************************************************************/
s16 Get_TempVal(void) {
	return gTemp_data;
}

/*******************************************************************************
 Function:System_Init
 Description:Init the device info to be a known start value (as defined at top of CTRL.c)
 Input:Void
 Output:Void
 *******************************************************************************/
void System_Init(void) {
	memcpy((void*) &device_info, (void*) &info_def, sizeof(device_info));
}
/*******************************************************************************
 Function:Pid_Init
 Description:Inits the PID values to defaults (0 usually)
 Input:Void
 Output:Void
 *******************************************************************************/
void Pid_Init(void) {
	pid.settemp = 0;
	pid.actualtemp = 0;
	pid.err = 0;
	pid.err_last = 0;
	pid.integral = 0;
	pid.ht_time = 0;
	pid.kp = 15;
	pid.ki = 2;
	pid.kd = 1;
}

/*******************************************************************************
 Function:Pid_Realize
 Description: Calculates the next heating value using the PID algorithum
 Input:Current temp from the tip
 Output:
 *******************************************************************************/
u16 Pid_Realize(s16 temp) {
	u8 index = 0, index1 = 1;
	s16 d_err = 0;

	pid.actualtemp = temp;
	pid.err = pid.settemp - pid.actualtemp;    //

	if (pid.err >= 500)
		index = 0;
	else {
		index = 1;
		pid.integral += pid.err;    //
	}
////////////////////////////////////////////////////////////////////////////////
//
	if (pid.settemp < pid.actualtemp) {
		d_err = pid.actualtemp - pid.settemp;
		if (d_err > 20) {
			pid.integral = 0; //
			index1 = 0;
			index = 0;
		}
	}
////////////////////////////////////////////////////////////////////////////////
	if (pid.err <= 30)
		index1 = 0;
	else
		index1 = 1;
	pid.ht_time = pid.kp * pid.err + pid.ki * index * pid.integral
			+ pid.kd * (pid.err - pid.err_last) * index1;
	pid.err_last = pid.err;

	if (pid.ht_time <= 0)
		pid.ht_time = 0;
	else if (pid.ht_time > 30 * 200)
		pid.ht_time = 30 * 200;

	return pid.ht_time;

}

/*******************************************************************************
 Function:Heating_Time
 Description:Calcuates the on time for the heating element
 Input: (temp) current Tip Temp, (wk_temp) current ideal setpoint temp
 Output: The ON time for the heater element
 *******************************************************************************/
u32 Heating_Time(s16 temp, s16 wk_temp) {
	u32 heat_timecnt;

	pid.settemp = wk_temp;
	if (wk_temp > temp) {
		if (wk_temp - temp >= 18)
			gHt_flag = 0; //����
		else
			gHt_flag = 2; //����
	} else {
		if (temp - wk_temp <= 18)
			gHt_flag = 2; //����
		else
			gHt_flag = 1; //����
	}

	heat_timecnt = Pid_Realize(temp); //Sub_data * 1000;

	return heat_timecnt;
}
/*******************************************************************************
 Function:Status_Tran
 Description: Handles the current status of the unit, and task selection
 Basically this is called in main() repeatedly
 Input:Void
 Output:Void
 *******************************************************************************/
void Status_Tran(void) //
{
	static u16 init_waitingtime = 0;
	static u8 back_prestatus = 0;
	s16 heat_timecnt = 0, wk_temp;
	u16 mma_active;

	switch (Get_CtrlStatus()) {
	case IDLE:
		switch (Get_gKey()) { //Read current switch positions
		case KEY_V1: //If V1 key is pressed
			if (gIs_restartkey != 1) { //check we are not in a soft restart situation
				if (Read_Vb(1) < 4) { //Read that the input voltage is acceptable??
					Set_CtrlStatus(TEMP_CTR); //Set to temperature controlled mode (Aka soldering mode)
					init_waitingtime = 0; //Initialize the wait count to 0
					TEMPSHOW_TIMER= 0; //Initialize the timer to 0
					UI_TIMER= 0;
					G6_TIMER= 0;
				}
			}
			break;
			case KEY_V2: //check if V2 key is pressed
			if(gIs_restartkey != 1) { //check this is not a soft restart situation
				Set_CtrlStatus(THERMOMETER);//Change system to Thermometer mode instead (ie reading temp only, no drive)
				UI_TIMER = 0;
				Set_LongKeyFlag(1);//Set the long key pressed flag??
			}
			break;
			case KEY_CN|KEY_V3: //If A&B pressed at the same time, no action
			break;
		}
		if(gIs_restartkey && (KD_TIMER == 0)) { //This is a soft restart situation instead
			gIs_restartkey = 0;//reset the flag for soft restart
			Set_gKey(NO_KEY);//reset keys pressed
		}
		if(Read_Vb(1) == 0) { //Invalid voltage, I think this means no input power detected
			if(Get_UpdataFlag() == 1) Set_UpdataFlag(0);
			Set_CtrlStatus(ALARM);
		}
		if(gPre_status != WAIT && gPre_status != IDLE) { //System has been left alone, turn off screen to stop burn in
			G6_TIMER = device_info.idle_time;
			Set_gKey(NO_KEY);
			gPre_status = IDLE;
		}
		break;
		case TEMP_CTR: //We are in soldering mode
		switch(Get_gKey()) { //switch on the  pressed key
			case KEY_CN|KEY_V1:
			case KEY_CN|KEY_V2://if either key long pressed
			Set_HeatingTime(0);//turn off heater
			Set_CtrlStatus(TEMP_SET);//Goto temperature set mode
			HEATING_TIMER = 0;//reset heating timer
			EFFECTIVE_KEY_TIMER = 500;
			break;
			case KEY_CN|KEY_V3://Both keys pressed
			Set_HeatingTime(0);//Stop the heater
			Set_LongKeyFlag(0);//Reset the long key press flag
			Set_CtrlStatus(IDLE);//Change the system back to IDLE state (stop soldering)
			gPre_status = TEMP_CTR;//Set previous status
			gIs_restartkey = 1;
			KD_TIMER = 50;//
			break;
		}

		if(Read_Vb(1) >= 4) { //Check input voltage is in the acceptable range
			Set_HeatingTime(0);//Turn of heater as we are out of range
			Set_LongKeyFlag(0);//reset key flag
			Set_CtrlStatus(IDLE);//reset to IDLE state
			gPre_status = TEMP_CTR;//set previous state
			gIs_restartkey = 1;
			KD_TIMER = 50;//
		}

		wk_temp = device_info.t_work; //update setpoint temp from the struct
		if(HEATING_TIMER == 0) {
			gTemp_data = Get_Temp(wk_temp);
			heat_timecnt = Heating_Time(gTemp_data,wk_temp); //Calculate the on time for the heating cycle
			Set_HeatingTime(heat_timecnt);//set the on time for the heating cycle
			HEATING_TIMER = HEATINGCYCLE;
		}

		if(Get_HeatingTime() == 0) { //If calcuated heater time is 0 stop the timer ?
			HEATING_TIMER = 0;
		}

		/*
		 * The logic here is :
		 * If the device is moving then disarm the timer and mark it as needed a re-init
		 * else check if the timer needs init, if it does set it up and exit
		 * 		if the timer does not need init, then check if the timer has expired (its a count down)
		 * 		If the timer has expired goto wait state instead and shutdown iron
		 */
		mma_active = Get_MmaShift(); //check the accelerometer for movement
		if(mma_active == 0) { //MMA_active = 0 ==> static ,MMA_active = 1 ==>move
			if(init_waitingtime == 0) { //If the waiting countdown timer is not initialized
				init_waitingtime = 1;//we initialize it and set this <- flag.
				ENTER_WAIT_TIMER = device_info.wait_time;
			}
			if((init_waitingtime != 0) && (ENTER_WAIT_TIMER == 0)) { //if timeout has been initalized and enter_wait_timer has reached 0
				gHt_flag = 0;//reset heating flags
				UI_TIMER = 0;//reset ui timers
				Set_HeatingTime(0);//turn off the soldering iron
				Set_gKey(0);//clear keys
				G6_TIMER = device_info.idle_time;//set the device to idle timer move
				Set_CtrlStatus(WAIT);//Set system mode to waiting for movement
			}
		} else { //The iron is moving
			init_waitingtime = 0;//mark the waiting timer for needing reset if movement stops again
		}

		if(Get_AlarmType() > NORMAL_TEMP) {   //
			if(Get_UpdataFlag() == 1) Set_UpdataFlag(0);
			Set_CtrlStatus(ALARM);//Change to alarm state
		}
		break;

		case WAIT:
		//This mode (WAIT) occures when the iron has been idling on a desk for too long (ie someone forgot it was left on)
		//In this state we drop to a lower, safer temp and wait for movement or button push to wake up to operating temp again
		wk_temp = device_info.t_standby;
		if(device_info.t_standby > device_info.t_work) {
			//Check if the set temp was greater than the idle temp, if it was we set the idle temp to the set temp
			//This is done to avoid standby going to a higher temp
			wk_temp = device_info.t_work;
		}
		//if the heating timer has expired, update the readings
		if(HEATING_TIMER == 0) {
			gTemp_data = Get_Temp(wk_temp);			//read the tip temp
			heat_timecnt = Heating_Time(gTemp_data,wk_temp);//calculate the new heating timer value from temps
			Set_HeatingTime(heat_timecnt);//apply the new heating timer
			HEATING_TIMER = 30;//set update rate for heating_timer
		}

		if(Read_Vb(1) >= 4) {			//If the input voltage is not valid
			Set_HeatingTime(0);//turn off heater
			Set_LongKeyFlag(0);//reset key press flag
			Set_CtrlStatus(IDLE);//goto IDLE state
			G6_TIMER = device_info.idle_time;
			gPre_status = WAIT;//set previous state
			gIs_restartkey = 1;
			KD_TIMER = 50;//
		}

		if(G6_TIMER == 0) { //
			Set_HeatingTime(0);
			Set_LongKeyFlag(0);
			gIs_restartkey = 1;
			KD_TIMER = 200;//
			gPre_status = WAIT;
			Set_CtrlStatus(IDLE);
		}

		//If movement has occurred OR a key has been pressed -> Wakeup back to soldering
		mma_active = Get_MmaShift();//read accelerometer
		if(mma_active == 1 || Get_gKey() != 0) {
			UI_TIMER = 0; //reset the un-needed timers
			G6_TIMER = 0;
			init_waitingtime = 0;
			Set_CtrlStatus(TEMP_CTR);//Go back to soldering iron mode
		}

		if(Get_AlarmType() > NORMAL_TEMP) {   //If an alarm has occurred??
			if(Get_UpdataFlag() == 1) Set_UpdataFlag(0);
			Set_CtrlStatus(ALARM);//goto alarm error state
		}
		break;

		case TEMP_SET:   //We are in the setting soldering iron temp mode
		if(EFFECTIVE_KEY_TIMER == 0) {
			gCalib_flag = 1;

			gCalib_flag = 0;
			Set_CtrlStatus(TEMP_CTR);   //return to soldering mode
			TEMPSHOW_TIMER = 0;//turn off the timer
		}
		break;

		case THERMOMETER: //we are measuring the tip temp without applying any power
		if(KD_TIMER > 0) {
			Set_gKey(NO_KEY);
			break;
		}

		switch(Get_gKey()) {
			case KEY_CN|KEY_V1:
			case KEY_CN|KEY_V2:
			back_prestatus = 1;
			break;
			case KEY_CN|KEY_V3:
			Zero_Calibration(); //Calibrate the temperature (i think??)
			if(Get_CalFlag() == 1) {
			}
			KD_TIMER = 200; //20150717 �޸�
			break;
			default:
			break;
		}
		if(back_prestatus == 1) { //we are exiting
			back_prestatus = 0;//clear flag
			Set_HeatingTime(0);//turn off heater? (not sure why this is done again)
			Set_CtrlStatus(IDLE);//Goto IDLE state
			gPre_status = THERMOMETER;//set previous state
			gIs_restartkey = 1;//signal soft restart required as we may have done a calibration
			Set_LongKeyFlag(0);//reset long key hold flag
			KD_TIMER = 50;//
		}
		break;
		case ALARM: //An error has occured so we are in alarm state
		switch(Get_AlarmType()) {
			case HIGH_TEMP: //over temp condition
			case SEN_ERR://sensor reading error
			wk_temp = device_info.t_work;
			gTemp_data = Get_Temp(wk_temp);
			if(Get_AlarmType() == NORMAL_TEMP) {
				Set_CtrlStatus(TEMP_CTR);
				Set_UpdataFlag(0);
			}
			break;
			case HIGH_VOLTAGE: //over voltage
			case LOW_VOLTAGE://under voltage
			if(Read_Vb(1) >= 1 && Read_Vb(1) <= 3) {
				Set_HeatingTime(0); //turn off heater
				Set_LongKeyFlag(0);//reset key flag
				gIs_restartkey = 1;
				UI_TIMER = 2;// 2��
				gPre_status = THERMOMETER;
				Set_CtrlStatus(IDLE);
			}
			break;
		}
		break;
		default:
		break;
	}
}

/******************************** END OF FILE *********************************/
