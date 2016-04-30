/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. **********************
File Name :      CTRL.c
Version :        S100 APP Ver 2.11   
Description:
Author :         Celery
Data:            2015/07/07
History:
2015/07/07   统一命名；
2015/07/20   加大温度报警上限
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "APP_Version.h"
#include "HARDWARE.h"
#include "CTRL.h"
#include "bios.h"
#include "HARDWARE.h"
#include "UI.h"
/******************************************************************************/
#define CAL_AD 250
const u32 gVol[] = {3900,2760,1720,584};
const u16 gRate[] = {300,150,90,40};
s32 gZerop_ad = 239;
u32 gTurn_offv = 100;
u8 gCalib_flag = 0;
vu16 gMeas_cnt= 0;/* Measure*/
u32 gKey_in;
u8 gLongkey_flag = 0;
u8 gAlarm_type = 1;
/*******************************************************************************
函数名: Get_CalFlag
函数作用:读取校准状态
输入参数:NULL
返回参数:校准状态标志
*******************************************************************************/
u32 Get_CalFlag(void)
{
    return gCalib_flag;
}

/*******************************************************************************
函数名: Get_gKey
函数作用:获取按键状态
输入参数:NULL
返回参数:按键状态
*******************************************************************************/
u32 Get_gKey(void)
{
    return gKey_in;
}
/*******************************************************************************
函数名: Set_gKey
函数作用:设置按键状态
输入参数:要设置的按键状态
返回参数:NULL
*******************************************************************************/
void Set_gKey(u32 key)
{
    gKey_in = key;
}
/*******************************************************************************
函数名: Set_LongKeyFlag
函数作用:设置长按键标志
输入参数:0 :不可以长按键 1: 可以长按
返回参数:NULL
*******************************************************************************/
void Set_LongKeyFlag(u32 flag)
{
    gLongkey_flag = flag;
}
/*******************************************************************************
函数名: Get_AlarmType
函数作用:获取报警类型
输入参数:NULL
返回参数: 警告类型
          0:正常
          1:sen - err
          2:超温
          3:超压
*******************************************************************************/
u8 Get_AlarmType(void)
{
    return gAlarm_type;
}
/*******************************************************************************
函数名: Set_AlarmType
函数作用:设置报警类型
输入参数: 警告类型
          0:正常
          1:sen - err
          2:超温
          3:超压
返回参数:NULL
*******************************************************************************/
void Set_AlarmType(u8 type)
{
    gAlarm_type = type;
}
/*******************************************************************************
函数名: Read_Vb
函数作用:读取电源电压值
输入参数:标志
返回参数:NULL
*******************************************************************************/
int Read_Vb(u8 flag)
{
    u32 tmp,i,sum = 0;

    for(i = 0; i < 10; i++) {
        tmp = ADC_GetConversionValue(ADC2);
        sum += tmp;
    }
    tmp = sum/10;
    if(tmp >= (gVol[0] + gVol[0]/100)) {
        gAlarm_type = HIGH_VOLTAGE;
        return H_ALARM;  //大于3500
    }
    tmp = (tmp*10/144);//电压vb = 3.3 * 85 *ad / 40950

    for(i = 0; i < 4; i++) {
        if(i == 2) {
            if(flag == 0 ) {
                if(tmp >= gRate[i])break;
            } else {
                if(tmp >= gTurn_offv)break;
            }
        } else {
            if(tmp >= gRate[i])break;
        }
    }
    return (i+1);
}
/*******************************************************************************
函数名: Scan_Key
函数作用:扫描键盘(50ms每次)
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Scan_Key(void)
{
    static u32 p_cnt = 0,key_statuslast = 0;
    u32 key_state = 0;

    if((~GPIOA->IDR) & 0x0200) key_state |= KEY_V1; //KEY_V1
    if((~GPIOA->IDR) & 0x0040) key_state |= KEY_V2; //KEY_V2

    if(key_state == 0)  return ;

    if(gLongkey_flag == 1) { //LongKey_flag :限制长按健标志
        if(key_statuslast == key_state) {
            p_cnt++;
            if(p_cnt > 21)    Set_gKey(KEY_CN | key_state);//长按键
        } else {
            p_cnt = 0;
            key_statuslast = key_state;
            Set_gKey(key_state);
        }
    } else {
        p_cnt = 0;
        key_statuslast = key_state;
        Set_gKey(key_state);
    }

}

/*******************************************************************************
函数名: Get_SlAvg
函数作用:滑动平均值
输入参数:avg_data 平均的AD值
返回参数:滑动平均值
*******************************************************************************/
u32 Get_SlAvg(u32 avg_data)
{
    static u32 sum_avg = 0;
    static u8 init_flag = 0;
    u16 si_avg = sum_avg/SI_COE,abs;

    if(init_flag == 0) { /*第一次上电*/
        sum_avg = SI_COE * avg_data;
        init_flag = 1;
        return sum_avg/SI_COE;
    }
    if (avg_data > si_avg)    abs = avg_data - si_avg;
    else                      abs = si_avg - avg_data;

    if(abs > SI_THRESHOLD)  sum_avg =   SI_COE * avg_data;
    else                    sum_avg +=  avg_data  - sum_avg/SI_COE;

    return sum_avg/SI_COE;
}

/*******************************************************************************
函数名: Get_AvgAd
函数作用:获取并计算热端AD平均值
输入参数:NULL
返回参数:AD平均值
*******************************************************************************/
u32 Get_AvgAd(void)
{
    static u32 ad_sum = 0;
    static u32 max = 0,min = 5000;
    u32 ad_value,avg_data,slide_data;
    
    Set_HeatingTime(0);
    HEAT_OFF();
    Delay_HalfMs(25);
    gMeas_cnt = 10;

    while(gMeas_cnt > 0) {
        ad_value = Get_AdcValue(0);//Read_Tmp();
        ad_sum += ad_value;
        if(ad_value > max)   max = ad_value;
        if(ad_value < min)   min = ad_value;

        if(gMeas_cnt == 1) {
            ad_sum    = ad_sum - max - min;
            avg_data  = ad_sum / 8;

            slide_data = Get_SlAvg(avg_data);
            ad_sum = 0;
            min = 5000;
            max = 0;
        }
        gMeas_cnt--;
    }
    return slide_data;//gSlide_data;
}

/*******************************************************************************
函数名: Get_TempSlAvg
函数作用:冷端温度滑动平均值
输入参数:avg_data 冷端温度平均值
返回参数:冷端温度滑动平均值
*******************************************************************************/
int Get_TempSlAvg(int avg_data)
{
    static int sum_avg = 0;
    static u8 init_flag = 0;

    if(init_flag == 0) { /*第一次上电*/
        sum_avg = 8 * avg_data;
        init_flag = 1;
        return sum_avg/8;
    }

    sum_avg += avg_data  - sum_avg/8;

    return sum_avg/8;
}

/*******************************************************************************
函数名: Get_SensorTmp
函数作用:获取冷端温度
输入参数:NULL
返回参数:获取冷端温度
*******************************************************************************/
int Get_SensorTmp(void)
{
    static u32 ad_sum = 0;
    static u32 max = 0,min = 5000;
    u32 ad_value,avg_data,slide_data;
    int sensor_temp = 0;

    gMeas_cnt = 10;

    while(gMeas_cnt > 0) {
        ad_value = Get_AdcValue(1);
        ad_sum += ad_value;
        if(ad_value > max)   max = ad_value;
        if(ad_value < min)   min = ad_value;

        if(gMeas_cnt == 1) {
            ad_sum    = ad_sum - max - min;
            avg_data  = ad_sum / 8;

            slide_data = Get_TempSlAvg(avg_data);
            sensor_temp = (250 + (3300*slide_data/4096)-750);//(25 + ((10*(33*gSlide_data)/4096)-75));
            ad_sum = 0;
            min = 5000;
            max = 0;
        }
        gMeas_cnt--;
    }
    return sensor_temp;
}

/*******************************************************************************
函数名: Zero_Calibration
函数作用:校准零点AD
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Zero_Calibration(void)
{
    u32 zerop;
    int cool_tmp;

    zerop     = Get_AvgAd();
    cool_tmp  = Get_SensorTmp();

    if(zerop >= 400) {
        gCalib_flag = 2;
    } else {
        if(cool_tmp < 300) {
            gZerop_ad = zerop;
            gCalib_flag = 1;
        } else {
            gCalib_flag = 2;
        }
    }
}
/******************************************************************************* 
函数名: Get_Temp
函数作用:根据冷端,热端温度,补偿AD计算温度
输入参数:wk_temp 工作温度
返回参数:实际温度
*******************************************************************************/
s16 Get_Temp(s16 wk_temp)
{
    int ad_value,cool_tmp,compensation = 0;
    static u16 cnt = 0,h_cnt = 0;
    s16 rl_temp = 0;

    ad_value = Get_AvgAd();
    cool_tmp = Get_SensorTmp();
    
    if(ad_value == 4095) h_cnt++;
    else {
        h_cnt = 0;
        if(ad_value > 3800 && ad_value < 4095) cnt++; //20150720修改
        else cnt = 0;
    }
    if(h_cnt >= 60&& cnt == 0)    gAlarm_type = SEN_ERR;   //Sen-err
    if(h_cnt == 0 && cnt >= 10)   gAlarm_type = HIGH_TEMP; //超温
    if(h_cnt < 60 && cnt < 10)    gAlarm_type = NORMAL_TEMP;
    
    compensation = 80 + 150 * (wk_temp - 1000) / 3000;
    if(wk_temp == 1000) compensation -= 10;
    
    if(wk_temp != 0) {
        if(ad_value > (compensation + gZerop_ad))   ad_value -= compensation;
    }
    if(cool_tmp > 400)  cool_tmp = 400;
    rl_temp = (ad_value * 1000 + 806 * cool_tmp - gZerop_ad * 1000)/806;
    
    return rl_temp;
}

/*******************************************************************************
函数名: Start_Watchdog
函数作用:初始化开门狗
输入参数:ms 开门狗计数
返回参数:返回1
*******************************************************************************/
u32 Start_Watchdog(u32 ms)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz (min:0.8ms -- max:3276.8ms */
    IWDG_SetPrescaler(IWDG_Prescaler_32);

    /* Set counter reload value to XXms */
    IWDG_SetReload(ms*10/8);

    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
    return 1;
}
/*******************************************************************************
函数名: Clear_Watchdog
函数作用:重置开门狗计数
输入参数:NULL
返回参数:返回1
*******************************************************************************/
u32 Clear_Watchdog(void)
{
    IWDG_ReloadCounter();
    return 1;
}
/******************************** END OF FILE *********************************/
