/*
 * Si7210_defines.h
 *
 *  Created on: 5 Oct. 2020
 *      Author: Ralim
 */

#ifndef CORE_DRIVERS_SI7210_DEFINES_H_
#define CORE_DRIVERS_SI7210_DEFINES_H_

#define SI7210_ADDRESS (0x30<<1)
#define SI7210_REG_ID 0xC0

/* Si7210 Register addresses */
#define SI7210_HREVID       0xC0U
#define SI7210_DSPSIGM      0xC1U
#define SI7210_DSPSIGL      0xC2U
#define SI7210_DSPSIGSEL    0xC3U
#define SI7210_POWER_CTRL   0xC4U
#define SI7210_ARAUTOINC    0xC5U
#define SI7210_CTRL1        0xC6U
#define SI7210_CTRL2        0xC7U
#define SI7210_SLTIME       0xC8U
#define SI7210_CTRL3        0xC9U
#define SI7210_A0           0xCAU
#define SI7210_A1           0xCBU
#define SI7210_A2           0xCCU
#define SI7210_CTRL4        0xCDU
#define SI7210_A3           0xCEU
#define SI7210_A4           0xCFU
#define SI7210_A5           0xD0U
#define SI7210_OTP_ADDR     0xE1U
#define SI7210_OTP_DATA     0xE2U
#define SI7210_OTP_CTRL     0xE3U
#define SI7210_TM_FG        0xE4U

/* Si7210 Register bit masks */
#define CHIP_ID_MASK        0xF0U
#define REV_ID_MASK         0x0FU
#define DSP_SIGSEL_MASK     0x07U
#define MEAS_MASK           0x80U
#define USESTORE_MASK       0x08U
#define ONEBURST_MASK       0x04U
#define STOP_MASK           0x02U
#define SLEEP_MASK          0x01U
#define ARAUTOINC_MASK      0x01U
#define SW_LOW4FIELD_MASK   0x80U
#define SW_OP_MASK          0x7FU
#define SW_FIELDPOLSEL_MASK 0xC0U
#define SW_HYST_MASK        0x3FU
#define SW_TAMPER_MASK      0xFCU
#define SL_FAST_MASK        0x02U
#define SL_TIMEENA_MASK     0x01U
#define DF_BURSTSIZE_MASK   0xE0U
#define DF_BW_MASK          0x1EU
#define DF_IIR_MASK         0x01U
#define OTP_READ_EN_MASK    0x02U
#define OTP_BUSY_MASK       0x01U
#define TM_FG_MASK          0x03U

#define DSP_SIGM_DATA_FLAG      0x80U
#define DSP_SIGM_DATA_MASK      0x7FU
#define DSP_SIGSEL_TEMP_MASK    0x01U
#define DSP_SIGSEL_FIELD_MASK   0x04U

/* Burst sizes */
#define DF_BW_1             0x0U << 1
#define DF_BW_2             0x1U << 1
#define DF_BW_4             0x2U << 1
#define DF_BW_8             0x3U << 1
#define DF_BW_16            0x4U << 1
#define DF_BW_32            0x5U << 1
#define DF_BW_64            0x6U << 1
#define DF_BW_128           0x7U << 1
#define DF_BW_256           0x8U << 1
#define DF_BW_512           0x9U << 1
#define DF_BW_1024          0xAU << 1
#define DF_BW_2048          0xBU << 1
#define DF_BW_4096          0xCU << 1
#define DF_BURSTSIZE_1      0x0U << 5
#define DF_BURSTSIZE_2      0x1U << 5
#define DF_BURSTSIZE_4      0x2U << 5
#define DF_BURSTSIZE_8      0x3U << 5
#define DF_BURSTSIZE_16     0x4U << 5
#define DF_BURSTSIZE_32     0x5U << 5
#define DF_BURSTSIZE_64     0x6U << 5
#define DF_BURSTSIZE_128    0x7U << 5



#endif /* CORE_DRIVERS_SI7210_DEFINES_H_ */
