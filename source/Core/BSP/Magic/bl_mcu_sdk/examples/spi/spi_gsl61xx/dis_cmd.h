#ifndef __DISPLAY_CMD__
#define __DISPLAY_CMD__

#include "ring_buffer.h"
extern Ring_Buffer_Type uartRB;

#define EXT_DAT_LEN 4
#define BUF_LEN     (EXT_DAT_LEN + 8)

#define CMD_INT          0x00
#define CMD_GETINFO      0x01
#define CMD_CAPTURE      0x02
#define CMD_ENROLL       0x03
#define CMD_ENROLL_EX    0x13
#define CMD_MATCH        0x04
#define CMD_MATCH_SINGLE 0x14
#define CMD_DELETE       0x05
#define CMD_TRANSMIT     0x06
#define CMD_CONFIG       0x07
#define CMD_DEBUG        0x08
#define CMD_INVALID      0xFF

#define SUCCESS_FP 0x0000
#define ERROR_FP   0x8000

#define ERROR_XOR    0x8F01
#define ERROR_SUM    0x8F02
#define ERROR_CMD    0x8F03
#define ERROR_PAR    0x8F04
#define ERROR_TIME   0x8F05
#define ERROR_NO_PAR 0x8F06
#define ERROR_SYS    0x8F07
#define ERROR_VDD1   0x8F09
#define ERROR_VDD2   0x8F0a
#define ERROR_FLASH  0x8F0b

#define FAIL_INITIAL 0x8101
#define FAIL_CALIB   0x8102

#define FAIL_DETECT_FINGER 0x8201
#define FAIL_CAPTURE       0x8202
#define FAIL_CREATE_TMP    0x8203

#define FAIL_BE_ENROLLED 0x8301
#define FAIL_MATCH_TMP1  0x8302
#define FAIL_MATCH_TMP2  0x8303
#define FAIL_TMP_FULL    0x8304
#define FAIL_ENROLL_1    0x8311
#define FAIL_ENROLL_2    0x8312

#define FAIL_NO_TMP    0x8401
#define FAIL_MATCH     0x8402
#define FAIL_MATCH_SUB 0x8403
#define FAIL_RESIDUAL  0x8404

#define FAIL_DELTE 0x8501

#define FAIL_MATCH_C_MAIN    0x9401
#define FAIL_MATCH_C_TMP1    0x9402
#define FAIL_MATCH_C_TMP2    0x9403
#define FAIL_MATCH_MAIN_TMP1 0x9404
#define FAIL_MATCH_MAIN_TMP2 0x9405
#define FAIL_MATCH_TMP1_TMP2 0x9406

#define FAIL_CMOS_TEST 0x8801

typedef struct
{
    uint8_t Head;
    uint8_t Cmd1;
    uint16_t Cmd2;
    uint16_t Len;
    uint8_t XOR;
    uint8_t ExtData[EXT_DAT_LEN];
    uint8_t SUM;
} PACK_TypeDef;

typedef struct
{
    uint8_t *Data;
    uint16_t Len;
} CMDPAR_TypeDef;

typedef union {
    uint8_t Buf[BUF_LEN];
    PACK_TypeDef Packet;
} FCSCMD_TypeDef;

void display_cmd_init(void);
void display_cmd_handle(void);

#endif