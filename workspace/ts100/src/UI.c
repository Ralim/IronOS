/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. **********************
 File Name :      UI.c
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
#include "APP_Version.h"
#include "UI.h"
#include "CTRL.h"
#include "WordLib.h"
#include "Bios.h"
#include "Oled.h"
#include "Hardware.h"

#include "MMA8652FC.h"
/******************************************************************************/
u8 gTemp_array[16 * 16 + 16];
u8 gTemperatureshowflag = 0;
u8 gUp_flag = 0, gDown_flag = 0, gLevel_flag = 0, gTempset_showctrl = 0;
u16 gTemp_array_u16[208];
u16 gSet_table[2] = { 4000, 1000 };
u32 gCont = 0;
/******************************************************************************/

/*******************************************************************************
 Function:
 Description:
 *******************************************************************************/
u32 Get_UpdataFlag(void) {
	return gCont;
}
/*******************************************************************************
 Function:
 Description:
 *******************************************************************************/
void Set_UpdataFlag(u32 cont) {
	gCont = cont;
}
/*******************************************************************************
 Function:
 Description:
 *******************************************************************************/
void Set_TemperatureShowFlag(u8 flag) {
	gTemperatureshowflag = flag;
}
/*******************************************************************************
 Function:
 Description:־
 *******************************************************************************/
u8 Get_TemperatureShowFlag(void) {
	return gTemperatureshowflag;
}
/*******************************************************************************
 Function:
 Description: Converts F to C or C to F.
 Input: flag = if flag==0 converts C to F
 *******************************************************************************/
s16 TemperatureShow_Change(u8 flag, s16 tmp) {
	if (flag == 0) {
		return (320 + tmp * 9 / 5);
	} else {
		return (5 * tmp - 1600) / 9;
	}
}
/*******************************************************************************
 Function:APP_Init
 Description:Init the operating mode by looking at input voltage to pick USB or normal
 *******************************************************************************/
void APP_Init(void) {
	int rev;

	HEATING_TIMER= 0;
	UI_TIMER= 0;

	rev = Read_Vb(0); //Read input voltage
	if (rev == 0) //no input volatage == error state
		Set_CtrlStatus(ALARM);
	else if (rev >= 4) { //We are USB powered (5V approx at input)
		Set_LongKeyFlag(1);
		Set_CtrlStatus(USB_POWER);
	} else { //Normal mode > ~9V at input
		Set_CtrlStatus(IDLE);
		G6_TIMER= device_info.idle_time;
	}

}

/*******************************************************************************
 Function:
 Description:
 *******************************************************************************/
void Display_Temp(u8 x, s16 temp) {
	char Str[8];

	memset(Str, 0x0, 6);
	sprintf(Str, "%d", temp);

	if (gTemperatureshowflag == 0) {
		if (temp > 99)
			Str[3] = 'C';
		else if (temp > 9) {
			Str[2] = 'C';
			Str[3] = ' ';
		} else if (temp < 0) {
			if (temp + 9 >= 0) {
				Str[2] = 'C';
				Str[3] = ' ';
			} else {
				Str[3] = 'C';
			}
		} else {
			Str[1] = 'C';
			Str[2] = ' ';
			Str[3] = ' ';
		}
	} else {
		if (temp > 99)
			Str[3] = 'E';
		else if (temp > 9) {
			Str[2] = 'E';
			Str[3] = ' ';
		} else if (temp < 0) {
			if (temp + 9 >= 0) {
				Str[2] = 'E';
				Str[3] = ' ';
			} else {
				Str[3] = 'E';
			}
		} else {
			Str[1] = 'E';
			Str[2] = ' ';
			Str[3] = ' ';
		}
	}
	Display_Str(x, Str);
	TEMPSHOW_TIMER= 20; //200ms
}
/*******************************************************************************
 Function:
 Description: Shows the press the button to start screen
 *******************************************************************************/
void Show_Notice(void) {
	int j, k;
	static u8* ptr0;
	static u8 posi = 0, i = 0;
//This draws the soldering iron logo to prompt the user to press the button to wake
	if (i == 0) { //1 Button up
		ptr0 = Oled_DrawArea(0, 0, 96, 16, (u8*) Maplib);
	} else if (i == 1) { //2 Button down
		ptr0 = Oled_DrawArea(0, 0, 96, 16, ptr0);
	} else if (i == 2) { //3 Button up
		ptr0 = Oled_DrawArea(0, 0, 96, 16, (u8*) Maplib);
	} else if (i == 3) { //4 Draw sliding text in
		for (j = 0; j < 6; j++) {
			k = 84;
			while (k >= posi) {
				ptr0 = (u8*) Maplib1 + j * 28;
				Clean_Char(k + 7, 14);
				ptr0 = Oled_DrawArea(k, 0, 14, 16, ptr0);
				k -= 7;
				Delay_Ms(10);
			}
			posi += 14;
		}
		posi = 0;
	}
	i++;
	if (i == 4)
		i = 0;
}

/*******************************************************************************
 Function:
 Description: Draws the word CONFIG to the screen
 *******************************************************************************/
void Show_Config(void) {
	u8* ptr;
	u8 j;

	ptr = (u8*) Config;
	for (j = 0; j < 6; j++) {
		ptr = Oled_DrawArea(j * 16, 0, 16, 16, ptr);
	}
}

/*******************************************************************************
 Function:
 Description:
 *******************************************************************************/
void Show_TempDown(s16 temp, s16 dst_temp) {
	static u8 guide_ui = 0;
	char str[8];

	memset(str, 0x0, 6);
	sprintf(str, "%d", temp);
	if (gTemperatureshowflag == 0) {
		if (temp > 99)
			str[3] = 'C';
		else {
			str[2] = 'C';
			str[3] = ' ';
		}
	} else {
		if (temp > 99)
			str[3] = 'E';
		else {
			str[2] = 'E';
			str[3] = ' ';
		}
	}

	Display_Str10(1, str);

	Oled_DrawArea(41, 0, 16, 16, (u8*) (Guide + (guide_ui++) * 32));

	if (guide_ui == 3)
		guide_ui = 0;

	memset(str, 0x0, 6);
	sprintf(str, "%d", dst_temp);
	if (gTemperatureshowflag == 0) {
		if (temp > 99)
			str[3] = 'C';
		else {
			str[2] = 'C';
			str[3] = ' ';
		}
	} else {
		if (temp > 99)
			str[3] = 'E';
		else {
			str[2] = 'E';
			str[3] = ' ';
		}
	}

	Display_Str10(56, str);
}
/*******************************************************************************
 Function:
 Description:
 *******************************************************************************/
void Show_Cal(u8 flag) {
	u8 i;
	u8* ptr = 0;

	if (flag == 1)
		ptr = (u8*) Cal_Done;
	else if (flag == 2)
		ptr = (u8*) Cal_UnDone;

	ptr = Oled_DrawArea(0, 0, 16, 16, (u8*) ptr);

	for (i = 1; i < 6; i++)
		ptr = Oled_DrawArea(16 * i, 0, 16, 16, (u8*) ptr);
	Delay_Ms(1000);
	Clear_Watchdog();
}
/*******************************************************************************
 Function:
 Description:
 *******************************************************************************/
void Show_Warning(void) {
	u8 i;
	u8* ptr = 0;
	static u8 flag = 0;

	switch (Get_AlarmType()) {
	case HIGH_TEMP:
		ptr = (u8*) Warning;
		break;
	case SEN_ERR:
		ptr = (u8*) SenErr;
		break;
	case HIGH_VOLTAGE:
		ptr = (u8*) HighVt;
		break;
	case LOW_VOLTAGE:
		ptr = (u8*) LowVot;
		break;
	}

	Oled_DrawArea(0, 0, 16, 16, (u8*) Warning + 20 * 7);
	if (flag == 0) {
		for (i = 2; i < 9; i++)
			Oled_DrawArea(10 * i, 0, 10, 16, (u8*) ptr + (i - 2) * 20);
		flag = 1;
	} else {
		Clean_Char(16, 80);
		flag = 0;
	}
}

/*****************************************************************
 Function: Show_Triangle
 Description:Draws in a triangle
 Input:(empty_trgl)

 empty_trgl: 0,1,2  ���Ӵ�,��Ӵ�,�ҼӴ�
 fill_trgl:  0,1,2  ��ʵ��,��ʵ��,��ʵ��
 *******************************************************************/
void Show_Triangle(u8 empty_trgl, u8 fill_trgl) {
	u8* ptr;
	ptr = (u8*) Triangle; //Get pointer to the pre-computed triangle array
	if ((empty_trgl == 0) && (fill_trgl == 0)) {
		ptr = Oled_DrawArea(0, 0, 16, 16, (u8*) ptr);
		ptr = Oled_DrawArea(5 * 16, 0, 16, 16, (u8*) ptr);
	} else if ((empty_trgl != 0) && (fill_trgl == 0)) {
		if (empty_trgl == 1) {
			ptr += 32;
			Oled_DrawArea(5 * 16, 0, 16, 16, (u8*) ptr);
			ptr += 32;
			Oled_DrawArea(0, 0, 16, 16, (u8*) ptr);
		} else if (empty_trgl == 2) {
			Oled_DrawArea(0, 0, 16, 16, (u8*) ptr);
			ptr += 32 * 3;
			Oled_DrawArea(5 * 16, 0, 16, 16, (u8*) ptr);
		}
	} else if ((empty_trgl == 0) && (fill_trgl != 0)) {
		if (fill_trgl == 1) {
			ptr += 32;
			Oled_DrawArea(5 * 16, 0, 16, 16, (u8*) ptr);
			ptr += 32 * 3;
			Oled_DrawArea(0, 0, 16, 16, (u8*) ptr);
		} else if (fill_trgl == 2) {
			Oled_DrawArea(0, 0, 16, 16, (u8*) ptr);
			ptr += 32 * 5;
			Oled_DrawArea(5 * 16, 0, 16, 16, (u8*) ptr);
		}
	}
}
/*******************************************************************************
 ������: Shift_Char
 ��������:�ַ����������ƶ���ָ��λ��
 �������:ptr �ƶ����ַ� pos �ƶ�����ָ��λ��
 ���ز���:NULL
 *******************************************************************************/
void Shift_Char(u8* ptr, u8 pos) {
	int k;
	u8* ptr0;

	k = 80;
	pos = pos * 16; //λ��*  �ַ�=   �ַ�����λ��
	while (k >= pos) {
		ptr0 = (u8*) ptr;
		Clean_Char(k + 16, 16);
		ptr0 = Oled_DrawArea(k, 0, 16, 16, (u8*) ptr0);
		k -= 16;
		Delay_Ms(25);
	}
}

/*******************************************************************************
 Function:Show_Set
 Description: Show the setpoint temp on the Oled
 *******************************************************************************/
void Show_Set(void) {
	u8 m, k;
	s16 num_temp;

	if (Get_TemperatureShowFlag() == 1) {
		num_temp = TemperatureShow_Change(0, device_info.t_work);
	} else {
		num_temp = device_info.t_work;
	}

	Shift_Char((u8*) Triangle + 0 * 32, 0);
	m = num_temp / 1000;   //��
	Shift_Char((u8*) wordlib + m * 32, 1);
	k = (num_temp - m * 1000) / 100;   //ʮ
	Shift_Char((u8*) wordlib + k * 32, 2);
	m = (num_temp - m * 1000 - k * 100) / 10;
	Shift_Char((u8*) wordlib + m * 32, 3);
	Shift_Char((u8*) wordlib + 13 * 32, 4);
	Shift_Char((u8*) Triangle + 1 * 32, 5);
}
/*******************************************************************************

 *******************************************************************************/
void Show_OrderChar(u8* ptr, u8 num, u8 width) {
	static u8 i = 1, j = 0, k = 0, m = 10;

	if (gLevel_flag == 0) { //ǰһ״̬���Ǻ���
		i = 1;
		j = 0;
		m = 8;
		gUp_flag = 0;
		gDown_flag = 0;
		gLevel_flag = 1;
	}

	if (i == 0) {
		for (k = 0; k <= m; k++) {
			gTemp_array[k] = *(ptr + 52 + j + k - 1);
			gTemp_array[k + width] = *(ptr + 52 + width + k + j - 1);
		}
		m++;
		j--;
		if (m == 11) {
			m--;
			j = 0;
			i = 1;
			return;
		}
	}

	if (j == 0) {
		for (k = 1; k < m; k++) {
			gTemp_array[k + i] = *(ptr + 52 + j + k - 1);
			gTemp_array[k + i + width] = *(ptr + 52 + width + k + j - 1);
		}
		m--;
		i++;
		if (m == 0) {
			m++;
			j = 9;
			i = 0;
		}
	}
}

/*******************************************************************************
 Function:Reverse_Bin8
 Description:Reverses the bit order of a uint8_t
 Input:Value to reverse
 Output:Reversed Value
 *******************************************************************************/
u8 Reverse_Bin8(u8 data) {
	u8 i, cache, result = 0;

	for (i = 0; i < 8; i++) {
		cache = data & (1 << i);
		if (i < 4) {
			cache <<= 7 - 2 * i;
		} else {
			cache >>= 2 * i - 7;
		}
		result += cache;
	}
	return result;
}
/*******************************************************************************

 *******************************************************************************/
void Show_ReverseChar(u8* ptr, u8 num, u8 width, u8 direction) {
	static u32 j = 0, m = 0, po_j[3] = { 0, 0, 0 }, po_m[3] = { 0, 0, 16 };
	u32 i, k;

	if (direction == 0) { //up
		if (gUp_flag == 0) { //
			j = 0;
			m = 0;
			gUp_flag = 1;
			gDown_flag = 0;
			gLevel_flag = 0;
		} else {
			j = po_j[0];
			m = po_m[0];
		}
	} else if (direction == 1) {
		if (gDown_flag == 0) { //
			j = 0;
			m = 0;
			gUp_flag = 0;
			gDown_flag = 1;
			gLevel_flag = 0;
		} else {
			j = po_j[1];
			m = po_m[1];
		}
	} else {
		j = po_j[2];
		m = po_m[2];
	}
	for (i = 0; i < width * 2 * num; i++)
		gTemp_array[i] = Reverse_Bin8(*(ptr + i)); //

	for (k = 0; k < width * 2 * num; k += width * 2)
		for (i = 0; i < width; i++) {
			gTemp_array_u16[i + k] = ((gTemp_array[i + k] & 0x00FF) << 8)
					| gTemp_array[i + k + width]; //
			if (direction == 1) {
				if (j == 0)
					gTemp_array_u16[i + k] <<= m; //
				else
					gTemp_array_u16[i + k] >>= j; //
			} else { //��
				if (m == 0)
					gTemp_array_u16[i + k] <<= j; //
				else
					gTemp_array_u16[i + k] >>= m; //
			}
			gTemp_array[i + k] = (gTemp_array_u16[i + k] & 0xFF00) >> 8;
			gTemp_array[i + k + width] = gTemp_array_u16[i + k] & 0x00FF;
		}

	for (i = 0; i < width * 2 * num; i++)
		gTemp_array[i] = Reverse_Bin8(gTemp_array[i]); //

	if (m == 0 && j == 16) {
		j = 0;
		m = 16;
	}
	if (m == 0)
		j++;
	else
		m--;

	if (direction == 0) { //up
		po_j[0] = j;
		po_m[0] = m;
	} else if (direction == 1) {
		po_j[1] = j;
		po_m[1] = m;
	} else {
		po_j[2] = j;
		po_m[2] = m;
	}
}

/*******************************************************************************
 Show_TempReverse
 word_num:
 word_width:
 direction :   ���� (0 up, 1 down)
 *******************************************************************************/
u8 Show_TempReverse(u8 num, u8 width, u8 direction) {
	static int i, j = 0, m = 16, k;
	u8 g, s, b;
	u8* ptr;
	s16 num_temp;

	if (Get_TemperatureShowFlag() == 1) {
		num_temp = TemperatureShow_Change(0, device_info.t_work);
	} else {
		num_temp = device_info.t_work;
	}

	num_temp = num_temp / 10;
	b = num_temp / 100;
	s = (num_temp - b * 100) / 10;
	g = (num_temp - b * 100 - s * 10);

	if (gTempset_showctrl == 1) {
		j = 1;
		m = 0;
		gTempset_showctrl = 0;
	} else if (gTempset_showctrl == 2) {
		j = 0;
		m = 16;
		gTempset_showctrl = 0;
	}
	for (i = 0; i < width * 2; i++) {
		gTemp_array[0 * 32 + i] = Reverse_Bin8(*(wordlib + b * 32 + i));
		gTemp_array[1 * 32 + i] = Reverse_Bin8(*(wordlib + s * 32 + i));
		gTemp_array[2 * 32 + i] = Reverse_Bin8(*(wordlib + g * 32 + i));
		if (Get_TemperatureShowFlag() == 1) {
			gTemp_array[3 * 32 + i] = Reverse_Bin8(*(wordlib + 15 * 32 + i));
		} else {
			gTemp_array[3 * 32 + i] = Reverse_Bin8(*(wordlib + 13 * 32 + i));
		}
	}

	for (k = (3 - num) * width * 2; k < width * 2 * 3; k += width * 2)
		for (i = 0; i < width; i++) {
			gTemp_array_u16[i + k] = ((gTemp_array[i + k] & 0x00FF) << 8)
					| gTemp_array[i + k + width];
			if (direction == 0) {
				if (m == 0)
					gTemp_array_u16[i + k] <<= j;
				else
					gTemp_array_u16[i + k] >>= m;
			} else {
				if (j == 0)
					gTemp_array_u16[i + k] <<= m;
				else
					gTemp_array_u16[i + k] >>= j;
			}
			gTemp_array[i + k] = (gTemp_array_u16[i + k] & 0xFF00) >> 8;
			gTemp_array[i + k + width] = gTemp_array_u16[i + k] & 0x00FF;
		}
	for (i = 0; i < width * 2; i++) {
		gTemp_array[0 * 32 + i] = Reverse_Bin8(gTemp_array[0 * 32 + i]);
		gTemp_array[1 * 32 + i] = Reverse_Bin8(gTemp_array[1 * 32 + i]);
		gTemp_array[2 * 32 + i] = Reverse_Bin8(gTemp_array[2 * 32 + i]);
		gTemp_array[3 * 32 + i] = Reverse_Bin8(gTemp_array[3 * 32 + i]);
	}

	ptr = (u8*) gTemp_array;
	for (i = 1; i <= 4; i++)
		ptr = Oled_DrawArea(16 * i, 0, 16, 16, (u8*) ptr);
	if ((m == 0 && j == 0) || (m == 0 && j == 16))
		return 0;
	if (m == 0)
		j++;
	else
		m--;
	return 1;
}

/*******************************************************************************
 Function: Show_HeatingIcon
 Draws the heating iron icon on the screen
 *******************************************************************************/
void Show_HeatingIcon(u32 ht_flag, u16 active) {
	u8* ptr;

	memset(gTemp_array, 0, 20);
	if (ht_flag == 0)
		Show_ReverseChar((u8*) TempIcon, 1, 10, 0); //(5,ptr,16);//����//
	else if (ht_flag == 1)
		Show_ReverseChar((u8*) TempIcon + 32, 1, 10, 1); //(5,ptr+32,16);//����//
	else if (ht_flag == 2)
		Show_OrderChar((u8*) TempIcon, 1, 10);  //(5,ptr+64,16);//����//

	ptr = (u8*) gTemp_array;
	Oled_DrawArea(86, 0, 10, 16, (u8*) ptr);

	if (Get_CtrlStatus() == WAIT)
		Oled_DrawArea(80, 0, 6, 16, (u8*) IhIcon + 12);
	else {
		if (active == 0)
			Oled_DrawArea(80, 0, 6, 16, (u8*) TempIcon + 20);
		else
			Oled_DrawArea(80, 0, 6, 16, (u8*) IhIcon);
	}
}
/*******************************************************************************
 Function:Display_Str
 Description: Draws certain chars to the screen at certain locations
 *******************************************************************************/
void Display_Str(u8 x, char* str) {
	u8* ptr;
	u8 temp;

	if ((x < 1) || (x > 8))
		x = 0;
	else
		x--;
	while (*str != 0) {
		temp = *str++;
		if (temp == ' ')
			temp = 10;
		else if (temp == '-')
			temp = 12;
		else {
			if ((temp >= 0x30) && (temp < 0x3a))
				temp = temp - 0x30; /* 0 --9 */
			else {
				if ((temp >= 0x41) && (temp <= 0x47))
					temp = temp - 0x41 + 11;/*ABD*/
				else
					return;
			}
		}
		ptr = (u8*) wordlib;
		ptr += temp * 32;
		Oled_DrawArea(x * 16, 0, 16, 16, (u8*) ptr);
		x++;
	}
}
/*******************************************************************************
 ������: Display_Str10
 ��������:��ʾ���Ϊ10���ַ���
 �������:x: λ�� str :��ʾ�ַ���
 ���ز���:NULL
 *******************************************************************************/
void Display_Str10(u8 x, char* str) {
	u8* ptr;
	u8 temp;
	u8 position = 0;

	while (*str != 0) {
		temp = *str++;
		if (temp == 'C')
			temp = 11;
		else if (temp == 'E')
			temp = 12;
		else {
			if ((temp >= 0x30) && (temp < 0x3a))
				temp = temp - 0x30; /* 0 --9 */
			else {
				temp = 10;
			}
		}
		ptr = (u8*) Number10;
		ptr += temp * 20;
		Oled_DrawArea(x + position * 10, 0, 10, 16, (u8*) ptr);
		position++;
	}
}

/*******************************************************************************
 ������: Print_Integer
 ��������:�������ݴ�ӡ����
 �������:data ����,posi λ��
 ���ز���:NULL
 *******************************************************************************/
void Print_Integer(s32 data, u8 posi) {
	char str[8];

	Clear_Screen();
	sprintf(str, "%ld", data);
	Display_Str(posi, str);
}
/*******************************************************************************
 ������: Calculation_TWork
 ��������:���������¶ȵ�ֵ(�Ӽ����ò�����Ľ��)
 �������:Flag (0+ or 1-)
 ���ز���:�¶ȼӼ����ò�����Ľ��
 *******************************************************************************/
u32 Calculation_TWork(u8 Flag) {
	s16 num_temp;

	if (Get_TemperatureShowFlag() == 1) {
		num_temp = TemperatureShow_Change(0, device_info.t_work);
		if (Flag == 0)
			num_temp += device_info.t_step;
		else
			num_temp -= device_info.t_step;
		return TemperatureShow_Change(1, num_temp);
	} else {
		if (Flag == 0)
			return (device_info.t_work + device_info.t_step);
		else
			return (device_info.t_work - device_info.t_step);
	}
}
/*******************************************************************************
 ������: Roll_Num
 ��������:�����У���Ҫ�������¶�ֵ��λ��
 �������:step ����,Flag (0+ or 1-)
 ���ز���:��Ҫ�����ĸ���
 *******************************************************************************/
u8 Roll_Num(u16 step, u8 flag) {
	u16 b1, b2, g1, g2;
	s16 num_temp;

	if (Get_TemperatureShowFlag() == 1) {
		num_temp = TemperatureShow_Change(0, device_info.t_work);
	} else {
		num_temp = device_info.t_work;
	}
	b2 = (num_temp) / 1000;
	g2 = (num_temp) / 100;

	if (flag == 0) {
		b1 = (num_temp + step) / 1000;
		g1 = (num_temp + step) / 100;
		if (b1 != b2)
			return 3;
		else if (g1 != g2)
			return 2;
		else
			return 1;
	} else {
		b1 = (num_temp - step) / 1000;
		g1 = (num_temp - step) / 100;

		if (b1 != b2)
			return 3;
		else if (g1 != g2)
			return 2;
		else
			return 1;
	}
}
/*******************************************************************************
 ������: Temp_SetProc
 ��������:�����¶�
 �������:NULL
 ���ز���:NULL
 *******************************************************************************/
void Temp_SetProc(void) {
	u8 theRoll_num = 3;
	static u16 cont = 0;

	if (device_info.t_step == 10)
		cont = 1;

	if (EFFECTIVE_KEY_TIMER> 0) {
		if(gCont != 0) {
			//���ý�������������
			gCont = 0;
			Set_LongKeyFlag(0);
			Clear_Screen();

			Show_Set();
		}
		switch(Get_gKey()) {
			case KEY_A:

			if(device_info.t_work > gSet_table[1]) {
				gTempset_showctrl = 1;
				theRoll_num = Roll_Num(device_info.t_step,1);
				while(Show_TempReverse(theRoll_num,16,1));
				device_info.t_work = Calculation_TWork(1);
				if(device_info.t_work < gSet_table[1]) device_info.t_work = gSet_table[1];
				gTempset_showctrl = 2;
				while(Show_TempReverse(theRoll_num,16,1));
				Show_Triangle(1,0);
			}
			if(device_info.t_work == gSet_table[1]) Show_Triangle(0,1);
			break;
			case KEY_B:

			if(device_info.t_work < gSet_table[0]) {
				gTempset_showctrl = 1;
				theRoll_num = Roll_Num(device_info.t_step,0);
				while(Show_TempReverse(theRoll_num,16,0));
				device_info.t_work = Calculation_TWork(0);
				if(device_info.t_work > gSet_table[0]) device_info.t_work = gSet_table[0];
				gTempset_showctrl = 2;
				while(Show_TempReverse(theRoll_num,16,0));
				Show_Triangle(2,0);
			}

			if(device_info.t_work == gSet_table[0]) Show_Triangle(0,2);
			break;
			default:
			break;
		}
		if(Get_gKey() > NO_KEY) {
			if(cont > 0 && EFFECTIVE_KEY_TIMER > 260) {
				device_info.t_step = 10 + cont * 50;
				cont++;
			}
			EFFECTIVE_KEY_TIMER = 300;
			Set_gKey(NO_KEY);
		}
		if(cont > 0 && EFFECTIVE_KEY_TIMER <= 260) {
			device_info.t_step = 10;
			cont = 1;
		}
	}
}

/*******************************************************************************
 Function:
 Description: This function draws in the appropriate content to the Oled as per the current program status
 *******************************************************************************/
void OLed_Display(void) {
	u16 ht_flag;
	s16 temp_val, dst_temp;
	static u8 td_flag = 0, config_show = 0;
	static u16 td_cnt = 0;
	static u16 bk = 0x33, id_cnt = 0;

	switch (Get_CtrlStatus()) {
	case IDLE:
		if (gCont == 1) {
			gCont = 0;
			Clear_Screen();
		}

		if (UI_TIMER== 0 && G6_TIMER != 0) {
			Show_Notice();
			UI_TIMER = 50;
		}
		if(G6_TIMER == 0) {
			id_cnt++;
			if(id_cnt == 50)Sc_Pt(bk--);
			if(bk == 0) Oled_DisplayOff();
		}
		if((Get_MmaShift() == 1) || (Get_gKey() != NO_KEY)) {
			G6_TIMER = device_info.idle_time; //reset the idle timer
			bk = 0x33;
			Sc_Pt(bk);
			Oled_DisplayOn();
		}
		break;
		case SOLDERING_MODE:
		if(gCont == 0) {
			gCont = 1;
			Set_LongKeyFlag(1);
			Clear_Screen();
		}
		ht_flag = Get_HtFlag();
		if(ht_flag != 1) {
			if(td_flag == 1) Clear_Screen();
			td_cnt = 0;
			td_flag = 0;
		}

		if(td_cnt == 75 && td_flag == 0) {
			Clear_Screen();
			td_flag = 1;
		}

		if(td_flag && UI_TIMER == 0) {
			temp_val = Get_TempVal();
			dst_temp = device_info.t_work;
			if(Get_TemperatureShowFlag() == 1) {
				temp_val = TemperatureShow_Change(0,temp_val);
				dst_temp = TemperatureShow_Change(0,dst_temp);
			}
			Show_TempDown(temp_val,dst_temp);
			UI_TIMER = 50;
		}
		if((TEMPSHOW_TIMER == 0) && (!td_flag)) {
			temp_val = Get_TempVal();
			if((temp_val > device_info.t_work) && (temp_val - device_info.t_work < 18)) temp_val = device_info.t_work;
			else if((temp_val <= device_info.t_work) && (device_info.t_work - temp_val < 18)) temp_val = device_info.t_work;
			if(Get_TemperatureShowFlag() == 1) {
				temp_val = TemperatureShow_Change(0,temp_val);
			}

			Display_Temp(1,temp_val/10);
			Show_HeatingIcon(ht_flag,Get_MmaActive());
			td_cnt++;
		}
		break;
		case TEMP_SET:
		Temp_SetProc();
		break;
		case USB_POWER:
		if(gCont == 1) {
			gCont = 0;
			Clear_Screen();
		}
		switch(Get_gKey()) {
			case KEY_CN|KEY_A:
			config_show = 1;
			break;
			case KEY_CN|KEY_B:
			config_show = 2;
			break;
			case KEY_CN|KEY_V3:
			config_show = 0;
			break;
			default:
			break;
		}
		if(config_show == 0) {
			Show_Config();
		}
		if(config_show != 3) {
			Set_gKey(NO_KEY);
			config_show = 3;
		}
		break;
		case THERMOMETER:
		if(gCont == 0) {
			gCont = 1;
			Clear_Screen();
		}
		if(gCalib_flag != 0) {
			//��ʾУ׼���
			Show_Cal(gCalib_flag);
			gCalib_flag = 0;
			Clear_Screen();
		}
		if(UI_TIMER == 0) {
			temp_val = Get_Temp(0);
			if(Get_TemperatureShowFlag() == 1) {
				temp_val = TemperatureShow_Change(0,temp_val);
			}
			Display_Temp(1,temp_val/10);
			Display_Str(6,"F");
			UI_TIMER = 20;
		}
		break;
		case ALARM:
		if(gCont == 0) {
			gCont = 1;
			Clear_Screen();
		}
		if(gCont == 1 && UI_TIMER == 0) {
			Show_Warning();
			UI_TIMER = 50;
		}
		break;
		case WAIT:
		temp_val = Get_TempVal();
		if((temp_val > device_info.t_standby) && (temp_val - device_info.t_standby <= 18)) temp_val = device_info.t_standby;
		else if((temp_val <= device_info.t_standby) && (device_info.t_standby - temp_val <= 18)) temp_val = device_info.t_standby;

		ht_flag = Get_HtFlag();

		if(ht_flag != 1) {
			if(td_flag == 1) Clear_Screen();
			td_cnt = 0;
			td_flag = 0;
		}

		if(td_cnt == 50 && td_flag == 0) {
			Clear_Screen();
			td_flag = 1;
		}

		if(td_flag && UI_TIMER == 0) {
			dst_temp = device_info.t_standby;
			if(Get_TemperatureShowFlag() == 1) {
				temp_val = TemperatureShow_Change(0,temp_val);
				dst_temp = TemperatureShow_Change(0,dst_temp);
			}
			Show_TempDown(temp_val,dst_temp);
			UI_TIMER = 50;
		}
		if((TEMPSHOW_TIMER == 0) && (!td_flag)) {
			if(Get_TemperatureShowFlag() == 1) {
				temp_val = TemperatureShow_Change(0,temp_val);
			}
			Display_Temp(1,temp_val/10);
			Show_HeatingIcon(ht_flag,Get_MmaActive()); //0����1����2����
			td_cnt++;
		}
		break;
		case SETTINGS_MENU:
		//We are in the menu structure.
		//We need to move through the selected menu items, or if the user has hit the EOL then jump back to IDLE

		break;
		default:
		break;
	}
}

/******************************** END OF FILE *********************************/
