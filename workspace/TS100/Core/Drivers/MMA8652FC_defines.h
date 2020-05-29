/*
 * MMA8652FC_defines.h
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */

#ifndef MMA8652FC_DEFINES_H_
#define MMA8652FC_DEFINES_H_

//--------------MMA8652 Registers-------------------------------------------//

#define STATUS_REG            0x00		// STATUS Register

#define OUT_X_MSB_REG         0x01		// [7:0] are 8 MSBs of the 14-bit X-axis sample
#define OUT_X_LSB_REG         0x02		// [7:2] are the 6 LSB of 14-bit X-axis sample
#define OUT_Y_MSB_REG         0x03		// [7:0] are 8 MSBs of the 14-bit Y-axis sample
#define OUT_Y_LSB_REG         0x04		// [7:2] are the 6 LSB of 14-bit Y-axis sample
#define OUT_Z_MSB_REG         0x05		// [7:0] are 8 MSBs of the 14-bit Z-axis sample
#define OUT_Z_LSB_REG         0x06		// [7:2] are the 6 LSB of 14-bit Z-axis sample

#define F_SETUP_REG           0x09    	// F_SETUP FIFO Setup Register
#define TRIG_CFG_REG          0x0A    	// TRIG_CFG Map of FIFO data capture events
#define SYSMOD_REG            0x0B    	// SYSMOD System Mode Register
#define INT_SOURCE_REG        0x0C    	// INT_SOURCE System Interrupt Status Register
#define WHO_AM_I_REG          0x0D    	// WHO_AM_I Device ID Register
#define XYZ_DATA_CFG_REG      0x0E    	// XYZ_DATA_CFG Sensor Data Configuration Register
#define HP_FILTER_CUTOFF_REG  0x0F    	// HP_FILTER_CUTOFF High Pass Filter Register

#define PL_STATUS_REG         0x10    	// PL_STATUS Portrait/Landscape Status Register
#define PL_CFG_REG            0x11    	// PL_CFG Portrait/Landscape Configuration Register
#define PL_COUNT_REG          0x12    	// PL_COUNT Portrait/Landscape Debounce Register
#define PL_BF_ZCOMP_REG       0x13    	// PL_BF_ZCOMP Back/Front and Z Compensation Register
#define P_L_THS_REG           0x14    	// P_L_THS Portrait to Landscape Threshold Register

#define FF_MT_CFG_REG         0x15    	// FF_MT_CFG Freefall and Motion Configuration Register
#define FF_MT_SRC_REG         0x16    	// FF_MT_SRC Freefall and Motion Source Register
#define FF_MT_THS_REG         0x17    	// FF_MT_THS Freefall and Motion Threshold Register
#define FF_MT_COUNT_REG       0x18    	// FF_MT_COUNT Freefall Motion Count Register

#define TRANSIENT_CFG_REG     0x1D    	// TRANSIENT_CFG Transient Configuration Register
#define TRANSIENT_SRC_REG     0x1E    	// TRANSIENT_SRC Transient Source Register
#define TRANSIENT_THS_REG     0x1F    	// TRANSIENT_THS Transient Threshold Register
#define TRANSIENT_COUNT_REG   0x20    	// TRANSIENT_COUNT Transient Debounce Counter Register

#define PULSE_CFG_REG         0x21    	// PULSE_CFG Pulse Configuration Register
#define PULSE_SRC_REG         0x22    	// PULSE_SRC Pulse Source Register
#define PULSE_THSX_REG        0x23    	// PULSE_THS XYZ Pulse Threshold Registers
#define PULSE_THSY_REG        0x24
#define PULSE_THSZ_REG        0x25
#define PULSE_TMLT_REG        0x26    	// PULSE_TMLT Pulse Time Window Register
#define PULSE_LTCY_REG        0x27    	// PULSE_LTCY Pulse Latency Timer Register
#define PULSE_WIND_REG        0x28    	// PULSE_WIND Second Pulse Time Window Register

#define ASLP_COUNT_REG        0x29    	// ASLP_COUNT Auto Sleep Inactivity Timer Register

#define CTRL_REG1             0x2A    	// CTRL_REG1 System Control 1 Register
#define CTRL_REG2             0x2B    	// CTRL_REG2 System Control 2 Register
#define CTRL_REG3             0x2C    	// CTRL_REG3 Interrupt Control Register
#define CTRL_REG4             0x2D    	// CTRL_REG4 Interrupt Enable Register
#define CTRL_REG5             0x2E    	// CTRL_REG5 Interrupt Configuration Register

#define OFF_X_REG             0x2F    	// XYZ Offset Correction Registers
#define OFF_Y_REG             0x30
#define OFF_Z_REG             0x31

//MMA8652FC 7-bit I2C address

#define MMA8652FC_I2C_ADDRESS (0x1D<<1)

//MMA8652FC Sensitivity

#define SENSITIVITY_2G		  1024
#define SENSITIVITY_4G		  512
#define SENSITIVITY_8G		  256

#define STATUS_REG    0x00
#define X_MSB_REG     0X01
#define X_LSB_REG     0X02
#define Y_MSB_REG     0X03
#define Y_LSB_REG     0X04
#define Z_MSB_REG     0X05
#define Z_LSB_REG     0X06

#define TRIG_CFG      0X0A
#define SYSMOD        0X0B
#define INT_SOURCE    0X0C
#define DEVICE_ID     0X0D

//-----STATUS_REG(0X00)-----Bit Define----------------------------------------//
#define ZYXDR_BIT     0X08
//----XYZ_DATA_CFG_REG(0xE)-Bit Define----------------------------------------//
#define FS_MASK       0x03
#define FULL_SCALE_2G 0x00      //2g=0x0,4g=0x1,8g=0x2
#define FULL_SCALE_4G 0x01
#define FULL_SCALE_8G 0x02
//---------CTRL_REG1(0X2A)Bit Define------------------------------------------//
#define ACTIVE_MASK   1<<0       //bit0
#define DR_MASK       0x38      //bit D5,D4,D3
#define FHZ800        0x0       //800hz
#define FHZ400        0x1       //400hz
#define FHZ200        0x2       //200hz
#define FHZ100        0x3       //100hz
#define FHZ50         0x4       //50hz
#define FHZ2          0x5       //12.5hz
#define FHZ1          0x6       //6.25hz
#define FHZ0          0x7       //1.563hz

//---------CTRL_REG2(0X2B)Bit Define------------------------------------------//
#define MODS_MASK     0x03     //Oversampling Mode 4
#define Normal_Mode   0x0      //Normal=0,Low Noise Low Power MODS=1,
//HI RESOLUTION=2,LOW POWER MODS = 11
//----CTRL_REG4---Interrupt Enable BIT ---------------------------------------//
//0 interrupt is disabled (default)
//1 interrupt is enabled
#define INT_EN_ASLP     1<<7    //Auto-SLEEP/WAKE Interrupt Enable
#define INT_EN_FIFO     1<<6    //FIFO Interrupt Enable
#define INT_EN_TRANS    1<<5    //Transient Interrupt Enable
#define INT_EN_LNDPRT   1<<4    //Orientation(Landscape/Portrait)Interrupt Enable
#define INT_EN_PULSE    1<<3    //Pulse Detection Interrupt Enable
#define INT_EN_FF_MT    1<<2    //Freefall/Motion Interrupt Enable
#define INT_EN_DRDY     1<<0    //Data Ready Interrupt Enable

#endif /* MMA8652FC_DEFINES_H_ */
