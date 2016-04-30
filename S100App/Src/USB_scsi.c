/******************** (C) COPYRIGHT 2015 e-Design Co., Ltd. ********************
 File Name : USB_scsi.c
 Version   : STM32 USB Disk Ver 3.4       Author : MCD Application Team & bure
*******************************************************************************/
#include "USB_scsi.h"
#include "USB_bot.h"
#include "USB_regs.h"
#include "usb_lib.h"
#include "Disk.h"

u8 Page00_Inquiry_Data[] ={ 0, 0, 0, 0, 0};
u8 Mode_Sense6_data[]    ={0x03, 0x00, 0x00, 0x00};
u8 Mode_Sense10_data[]   ={0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 Scsi_Sense_Data[]     ={0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 ReadCapacity10_Data[] ={ 0, 0, 0, 0, 0, 0, 0, 0};
u8 ReadFormatCapacity[]  ={ 0, 0, 0, 8, 0, 0, 0, 0, 2, 0, 0, 0};

#ifdef DFU_MODE
  uc8 Disk_Inquiry_Str[] ={0x00, 0x80, 0x02, 0x02, 36-4, 0x00, 0x00, 0x00,
                            'V',  'i',  'r',  't',  'u',  'a',  'l',  ' ',
                            'D',  'F',  'U',  ' ',  'D',  'i',  's',  'k',
                            ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',
                            ' ',  ' ',  ' ',  ' ', };
#else
  uc8 Disk_Inquiry_Str[] ={0x00, 0x80, 0x02, 0x02, 36-4, 0x00, 0x00, 0x00,
                            'M',  'i',  'n',  'i',  ' ',  'D',  'S',  'O',
                            'D',  'i',  's',  'k',  ' ',  ' ',  ' ',  ' ',
                            ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',
                            ' ',  ' ',  ' ',  ' ', };
#endif

/*******************************************************************************
  SCSI_Inquiry_Cmd: SCSI Inquiry Command routine.
*******************************************************************************/
void SCSI_Inquiry_Cmd(void)
{
  u8* Inquiry_Data;
  u16 Inquiry_Data_Length;

  if (CBW.CB[1] & 0x01){ // Evpd is set
    Inquiry_Data = Page00_Inquiry_Data;
    Inquiry_Data_Length = 5;
  } else {
    Inquiry_Data = (u8*)Disk_Inquiry_Str;
    if (CBW.CB[4] <= STANDARD_INQUIRY_DATA_LEN)  Inquiry_Data_Length = CBW.CB[4];
    else  Inquiry_Data_Length = STANDARD_INQUIRY_DATA_LEN;
  }
  Transfer_Data_Request(Inquiry_Data, Inquiry_Data_Length);
}
/*******************************************************************************
  SCSI_ReadFormatCapacity_Cmd: SCSI ReadFormatCapacity Command routine.
*******************************************************************************/
void SCSI_ReadFormatCapacity_Cmd(void)
{
  ReadFormatCapacity[ 4] = (u8)(SECTOR_SIZE >> 24);
  ReadFormatCapacity[ 5] = (u8)(SECTOR_SIZE >> 16);
  ReadFormatCapacity[ 6] = (u8)(SECTOR_SIZE >>  8);
  ReadFormatCapacity[ 7] = (u8)(SECTOR_SIZE);

  ReadFormatCapacity[ 9] = (u8)(SECTOR_SIZE >> 16);
  ReadFormatCapacity[10] = (u8)(SECTOR_SIZE >>  8);
  ReadFormatCapacity[11] = (u8)(SECTOR_SIZE);

  Transfer_Data_Request(ReadFormatCapacity, READ_FORMAT_CAPACITY_DATA_LEN);
}
/*******************************************************************************
  SCSI_ReadCapacity10_Cmd: SCSI ReadCapacity10 Command routine.
*******************************************************************************/
void SCSI_ReadCapacity10_Cmd(void)
{
  ReadCapacity10_Data[0] = (u8)(SECTOR_CNT - 1 >> 24);
  ReadCapacity10_Data[1] = (u8)(SECTOR_CNT - 1 >> 16);
  ReadCapacity10_Data[2] = (u8)(SECTOR_CNT - 1 >>  8);
  ReadCapacity10_Data[3] = (u8)(SECTOR_CNT - 1);

  ReadCapacity10_Data[4] = (u8)(SECTOR_SIZE >>  24);
  ReadCapacity10_Data[5] = (u8)(SECTOR_SIZE >>  16);
  ReadCapacity10_Data[6] = (u8)(SECTOR_SIZE >>  8);
  ReadCapacity10_Data[7] = (u8)(SECTOR_SIZE);

  Transfer_Data_Request(ReadCapacity10_Data, READ_CAPACITY10_DATA_LEN);
}
/*******************************************************************************
  SCSI_ModeSense6_Cmd: SCSI ModeSense6 Command routine.
*******************************************************************************/
void SCSI_ModeSense6_Cmd (void)
{
  Transfer_Data_Request(Mode_Sense6_data, MODE_SENSE6_DATA_LEN);
}
/*******************************************************************************
  SCSI_ModeSense10_Cmd: SCSI ModeSense10 Command routine.
*******************************************************************************/
void SCSI_ModeSense10_Cmd (void)
{
  Transfer_Data_Request(Mode_Sense10_data, MODE_SENSE10_DATA_LEN);
}
/*******************************************************************************
  SCSI_RequestSense_Cmd: SCSI RequestSense Command routine.
*******************************************************************************/
void SCSI_RequestSense_Cmd (void)
{
  u8 Request_Sense_data_Length;

  if (CBW.CB[4] <= REQUEST_SENSE_DATA_LEN)  Request_Sense_data_Length = CBW.CB[4];
  else  Request_Sense_data_Length = REQUEST_SENSE_DATA_LEN;

  Transfer_Data_Request(Scsi_Sense_Data, Request_Sense_data_Length);
}

/*******************************************************************************
  Set_Scsi_Sense_Data: Set Scsi Sense Data routine.
*******************************************************************************/
void Set_Scsi_Sense_Data(u8 Sens_Key, u8 Asc)
{
  Scsi_Sense_Data[2] = Sens_Key;
  Scsi_Sense_Data[12] = Asc;
}
/*******************************************************************************
  SCSI_Start_Stop_Unit_Cmd: SCSI Start_Stop_Unit Command routine.
*******************************************************************************/
void SCSI_Start_Stop_Unit_Cmd(void)
{
  Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
}
/*******************************************************************************
  SCSI_Read10_Cmd: SCSI Read10 Command routine.
*******************************************************************************/
void SCSI_Read10_Cmd(u32 LBA , u32 BlockNbr)
{
  if (Bot_State == BOT_IDLE){
    if (!(SCSI_Address_Management(SCSI_READ10, LBA, BlockNbr))) return;//address out of range
    if ((CBW.bmFlags & 0x80) != 0){
      Bot_State = BOT_DATA_IN;
      Read_Memory(LBA , BlockNbr);
    } else {
      Bot_Abort(BOTH_DIR);
      Set_Scsi_Sense_Data(ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
      Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
    }
    return;
  } else if (Bot_State == BOT_DATA_IN)  Read_Memory(LBA , BlockNbr);
}
/*******************************************************************************
  SCSI_Write10_Cmd: SCSI Write10 Command routine.
*******************************************************************************/
void SCSI_Write10_Cmd(u32 LBA , u32 BlockNbr)
{
  if (Bot_State == BOT_IDLE){
    if (!(SCSI_Address_Management(SCSI_WRITE10 , LBA, BlockNbr)))  return;//address out of range
    if ((CBW.bmFlags & 0x80) == 0){
      Bot_State = BOT_DATA_OUT;
      SetEPRxStatus(ENDP2, EP_RX_VALID);
    } else {
      Bot_Abort(DIR_IN);
      Set_Scsi_Sense_Data(ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
      Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
    }
    return;
  } else if (Bot_State == BOT_DATA_OUT)  Write_Memory(LBA , BlockNbr);
}
/*******************************************************************************
  SCSI_Verify10_Cmd: SCSI Verify10 Command routine.
*******************************************************************************/
void SCSI_Verify10_Cmd(void)
{
  if ((CBW.dDataLength == 0) && !(CBW.CB[1] & BLKVFY)){ // BLKVFY not set
    Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
  } else {
    Bot_Abort(BOTH_DIR);
    Set_Scsi_Sense_Data(ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
    Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
  }
}
/*******************************************************************************
  SCSI_Valid_Cmd: Valid Commands routine.
*******************************************************************************/
void SCSI_Valid_Cmd(void)
{
  if (CBW.dDataLength != 0){
    Bot_Abort(BOTH_DIR);
    Set_Scsi_Sense_Data(ILLEGAL_REQUEST, INVALID_COMMAND);
    Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
  } else  Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
}
/*******************************************************************************
  SCSI_Valid_Cmd: Valid Commands routine.
*******************************************************************************/
void SCSI_TestUnitReady_Cmd(void)
{
  Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
}
/*******************************************************************************
  SCSI_Invalid_Cmd: Invalid Commands routine
*******************************************************************************/
void SCSI_Invalid_Cmd(void)
{
  if (CBW.dDataLength == 0)  Bot_Abort(DIR_IN);
  else{
    if ((CBW.bmFlags & 0x80) != 0)  Bot_Abort(DIR_IN);
    else                            Bot_Abort(BOTH_DIR);
  }
  Set_Scsi_Sense_Data(ILLEGAL_REQUEST, INVALID_COMMAND);
  Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
}
/*******************************************************************************
  SCSI_Address_Management: Test the received address.
  Input:                   Cmd the command can be SCSI_READ10 or SCSI_WRITE10.
  Return:                  Read\Write status (bool).
*******************************************************************************/
u8 SCSI_Address_Management(u8 Cmd , u32 LBA , u32 BlockNbr)
{

  if ((LBA + BlockNbr) > SECTOR_CNT){
    if (Cmd == SCSI_WRITE10)  Bot_Abort(BOTH_DIR);
    Bot_Abort(DIR_IN);
    Set_Scsi_Sense_Data(ILLEGAL_REQUEST, ADDRESS_OUT_OF_RANGE);
    Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
    return (FALSE);
  }
  if (CBW.dDataLength != BlockNbr * SECTOR_SIZE){
    if (Cmd == SCSI_WRITE10) Bot_Abort(BOTH_DIR);
    else                     Bot_Abort(DIR_IN);
    Set_Scsi_Sense_Data(ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
    Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
    return (FALSE);
  }
  return (TRUE);
}
/*********************************  END OF FILE  ******************************/
