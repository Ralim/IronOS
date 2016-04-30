/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. **********************
File Name :      I2C.h
Version :        S100 APP Ver 2.11   
Description:
Author :         Celery
Data:            2015/08/03
History:
2015/08/03  : 
*******************************************************************************/
#ifndef __I2C_H
#define __I2C_H

#define I2C_TX    1
#define I2C_RX    2

void I2C_Configuration(void);
void Delay_uS(u32 us);
void I2C_PageWrite(u8* pbuf, u8 numbyte,u8 deviceaddr );
void I2C_PageRead(u8* pbuf,  u8 numbyte,u8 deviceaddr, u8 readaddr);
#endif
/******************************** END OF FILE *********************************/
