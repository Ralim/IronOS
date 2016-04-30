/******************** (C) COPYRIGHT 2013 e-Design Co., Ltd. ********************
 File Name : USB_bot.h
 Version   : STM32 USB Disk Ver 3.4       Author : MCD Application Team & bure
*******************************************************************************/
#ifndef __USB_BOT_H
#define __USB_BOT_H

typedef struct _Bulk_Only_CBW
{
  u32 dSignature;
  u32 dTag;
  u32 dDataLength;
  u8  bmFlags;
  u8  bLUN;
  u8  bCBLength;
  u8  CB[16];
}
Bulk_Only_CBW;

/* Bulk-only Command Status Wrapper */
typedef struct _Bulk_Only_CSW
{
  u32 dSignature;
  u32 dTag;
  u32 dDataResidue;
  u8  bStatus;
}
Bulk_Only_CSW;

#define BOT_IDLE                      0       /* Idle state */
#define BOT_DATA_OUT                  1       /* Data Out state */
#define BOT_DATA_IN                   2       /* Data In state */
#define BOT_DATA_IN_LAST              3       /* Last Data In Last */
#define BOT_CSW_Send                  4       /* Command Status Wrapper */
#define BOT_ERROR                     5       /* error state */

#define BOT_CBW_SIGNATURE             0x43425355
#define BOT_CSW_SIGNATURE             0x53425355
#define BOT_CBW_PACKET_LENGTH         31

#define CSW_DATA_LENGTH               0x000D

/* CSW Status Definitions */
#define CSW_CMD_PASSED                0x00
#define CSW_CMD_FAILED                0x01
#define CSW_PHASE_ERROR               0x02

#define SEND_CSW_DISABLE              0
#define SEND_CSW_ENABLE               1

#define DIR_IN                        0
#define DIR_OUT                       1
#define BOTH_DIR                      2

#define BULK_MAX_PACKET_SIZE          0x00000040

extern u8 Bot_State;
extern u16 Data_Len;
extern u8 Bot_State;
extern Bulk_Only_CBW CBW;
extern Bulk_Only_CSW CSW;
extern u8 Bulk_Buff[];

void Mass_Storage_In (void);
void Mass_Storage_Out (void);
void CBW_Decode(void);
void Transfer_Data_Request(u8* Data_Pointer, u16 Data_Len);
void Set_CSW (u8 CSW_Status, u8 Send_Permission);
void Bot_Abort(u8 Direction);

#endif
/*********************************  END OF FILE  ******************************/

