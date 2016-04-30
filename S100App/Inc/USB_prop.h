/******************** (C) COPYRIGHT 2015 e-Design Co., Ltd. ********************
 File Name : USB_prop.h  
 Version   : STM32 USB Disk Ver 3.4       Author : MCD Application Team & bure
*******************************************************************************/
#ifndef __usb_prop_H
#define __usb_prop_H

#include "USB_core.h"

#ifdef STM32F30X
  #include "STM32F30x.h"
#elif STM32F10X_HD
  #include "STM32F10x.h"
#endif

#define Mass_Storage_GetConfiguration          NOP_Process
/* #define Mass_Storage_SetConfiguration          NOP_Process*/
#define Mass_Storage_GetInterface              NOP_Process
#define Mass_Storage_SetInterface              NOP_Process
#define Mass_Storage_GetStatus                 NOP_Process
/* #define Mass_Storage_ClearFeature              NOP_Process*/
#define Mass_Storage_SetEndPointFeature        NOP_Process
#define Mass_Storage_SetDeviceFeature          NOP_Process
/*#define Mass_Storage_SetDeviceAddress          NOP_Process*/

/* MASS Storage Requests*/
#define GET_MAX_LUN                0xFE
#define MASS_STORAGE_RESET         0xFF
#define LUN_DATA_LENGTH            1

extern u32 Max_Lun;

void MASS_init(void);
void MASS_Reset(void);
void Mass_Storage_SetConfiguration(void);
void Mass_Storage_ClearFeature(void);
void Mass_Storage_SetDeviceAddress (void);
void MASS_Status_In (void);
void MASS_Status_Out (void);
RESULT MASS_Data_Setup(u8);
RESULT MASS_NoData_Setup(u8);
RESULT MASS_Get_Interface_Setting(u8 Interface, u8 AlternateSetting);
u8 *MASS_GetDeviceDescriptor(u16 );
u8 *MASS_GetConfigDescriptor(u16);
u8 *MASS_GetStringDescriptor(u16);
u8 *Get_Max_Lun(u16 Length);
void Get_SerialNum(void);

#endif 
/*********************************  END OF FILE  ******************************/

