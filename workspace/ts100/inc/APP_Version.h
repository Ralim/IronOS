/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. ********************                       
 File Name :      APP_Version.h
 Version :        S100 APP Ver 2.11
 Description:
 Author :         bure & Celery
 Data:            2015/08/03
 History:
 2015/08/03   ͳһ������
 *******************************************************************************/

#ifndef __VERSION_H
#define __VERSION_H

#define M_VER              'V','0'
#define S_VER              '0','1'
#define OEM_TYPE           "0"
#define APP_BASE           0x0800C000

/*----------------------------------------------------------------------------*/
#ifdef TYPE_DS201
#define PRODUCT_INFO   "DS201"
#define SCH_VER        "1.7B"
#else
	#ifdef TYPE_DS202
		#include "DS202V1_6.h"
		#define FLASH_PAGE       0x0800
	#else
		#ifdef TYPE_DS203
			#define PRODUCT_INFO     "DS203"
			#define SCH_VER          "2.70"
		#else
			#ifdef TYPE_S100
				#define PRODUCT_INFO     "TS100"
				#include "S100V0_1.h"
			#endif
		#endif
	#endif
#endif

/*----------------------------------------------------------------------------*/
#define DFU_VER            {M_VER,'.',S_VER,LCD_M, 0}
#endif /* VERSION_H */
/*********************************  END OF FILE  ******************************/
