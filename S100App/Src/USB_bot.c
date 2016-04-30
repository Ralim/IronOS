/******************** (C) COPYRIGHT 2015 e-Design Co., Ltd. ********************
 File Name : USB_bot.c
 Version   : STM32_USB Disk Ver 3.4        Author : MCD Application Team & bure
*******************************************************************************/
#include "USB_scsi.h"
#include "USB_regs.h"
#include "USB_mem.h"
#include "USB_conf.h"
#include "USB_bot.h"
#include "USB_prop.h"

u8 Bot_State;
u8 Bulk_Buff[BULK_MAX_PACKET_SIZE];  // Data_ data buffer
u16 Data_Len;
Bulk_Only_CBW CBW;
Bulk_Only_CSW CSW;
u32 SCSI_LBA , SCSI_BlkLen;

/*******************************************************************************
  Mass_Storage_In: Mass Storage IN transfer.
*******************************************************************************/
void Mass_Storage_In (void)
{
  switch (Bot_State)
  {
    case BOT_CSW_Send:
    case BOT_ERROR:
      Bot_State = BOT_IDLE;
      SetEPRxStatus(ENDP2, EP_RX_VALID);/* enable the Endpoint to recive the next cmd*/
      break;
    case BOT_DATA_IN:
      switch (CBW.CB[0])
      {
        case SCSI_READ10:
          SCSI_Read10_Cmd(SCSI_LBA , SCSI_BlkLen);
          break;
      }
      break;
    case BOT_DATA_IN_LAST:
      Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
      SetEPRxStatus(ENDP2, EP_RX_VALID);
      break;

    default:
      break;
  }
}
/*******************************************************************************
  Mass_Storage_Out: Mass Storage OUT transfer.
*******************************************************************************/
void Mass_Storage_Out (void)
{
  u8 CMD;
  CMD = CBW.CB[0];
  Data_Len = GetEPRxCount(ENDP2);

  PMAToUserBufferCopy(Bulk_Buff, ENDP2_RXADDR, Data_Len);

  switch (Bot_State)
  {
    case BOT_IDLE:
      CBW_Decode();
      break;
    case BOT_DATA_OUT:
      if (CMD == SCSI_WRITE10)
      {
        SCSI_Write10_Cmd(SCSI_LBA , SCSI_BlkLen);
        break;
      }
      Bot_Abort(DIR_OUT);
      Set_Scsi_Sense_Data(ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
      Set_CSW (CSW_PHASE_ERROR, SEND_CSW_DISABLE);
      break;
    default:
      Bot_Abort(BOTH_DIR);
      Set_Scsi_Sense_Data(ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
      Set_CSW (CSW_PHASE_ERROR, SEND_CSW_DISABLE);
      break;
  }
}
/*******************************************************************************
  CBW_Decode: Decode the received CBW and call the related SCSI command
*******************************************************************************/
void CBW_Decode(void)
{
  u32 Counter;

  for (Counter = 0; Counter < Data_Len; Counter++)
  {
    *((u8 *)&CBW + Counter) = Bulk_Buff[Counter];
  }
  CSW.dTag = CBW.dTag;
  CSW.dDataResidue = CBW.dDataLength;
  if (Data_Len != BOT_CBW_PACKET_LENGTH)
  {
    Bot_Abort(BOTH_DIR);
    // reset the CBW.dSignature to desible the clear feature until receiving a Mass storage reset
    CBW.dSignature = 0;
    Set_Scsi_Sense_Data(ILLEGAL_REQUEST, PARAMETER_LIST_LENGTH_ERROR);
    Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
    return;
  }

  if ((CBW.CB[0] == SCSI_READ10 ) || (CBW.CB[0] == SCSI_WRITE10 ))
  {
    // Calculate Logical Block Address
    SCSI_LBA = (CBW.CB[2] << 24) | (CBW.CB[3] << 16) | (CBW.CB[4] <<  8) | CBW.CB[5];
    // Calculate the Number of Blocks to transfer
    SCSI_BlkLen = (CBW.CB[7] <<  8) | CBW.CB[8];
  }

  if (CBW.dSignature == BOT_CBW_SIGNATURE)
  {
    // Valid CBW
    if ((CBW.bLUN > Max_Lun) || (CBW.bCBLength < 1) || (CBW.bCBLength > 16))
    {
      Bot_Abort(BOTH_DIR);
      Set_Scsi_Sense_Data(ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
      Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
    }
    else
    {
      switch (CBW.CB[0])
      {
        case SCSI_REQUEST_SENSE:
          SCSI_RequestSense_Cmd ();
          break;
        case SCSI_INQUIRY:
          SCSI_Inquiry_Cmd();
          break;
        case SCSI_START_STOP_UNIT:
          SCSI_Start_Stop_Unit_Cmd();
          break;
        case SCSI_ALLOW_MEDIUM_REMOVAL:
          SCSI_Start_Stop_Unit_Cmd();
          break;
        case SCSI_MODE_SENSE6:
          SCSI_ModeSense6_Cmd ();
          break;
        case SCSI_MODE_SENSE10:
          SCSI_ModeSense10_Cmd ();
          break;
        case SCSI_READ_FORMAT_CAPACITIES:
          SCSI_ReadFormatCapacity_Cmd();
          break;
        case SCSI_READ_CAPACITY10:
          SCSI_ReadCapacity10_Cmd();
          break;
        case SCSI_TEST_UNIT_READY:
          SCSI_TestUnitReady_Cmd();
          break;
        case SCSI_READ10:
          SCSI_Read10_Cmd(SCSI_LBA , SCSI_BlkLen);
          break;
        case SCSI_WRITE10:
          SCSI_Write10_Cmd(SCSI_LBA , SCSI_BlkLen);
          break;
        case SCSI_VERIFY10:
          SCSI_Verify10_Cmd();
          break;
          //Unsupported command
        case SCSI_MODE_SELECT10:
          SCSI_Mode_Select10_Cmd();
          break;
        case SCSI_MODE_SELECT6:
          SCSI_Mode_Select6_Cmd();
          break;

        case SCSI_SEND_DIAGNOSTIC:
          SCSI_Send_Diagnostic_Cmd();
          break;
        case SCSI_READ6:
          SCSI_Read6_Cmd();
          break;
        case SCSI_READ12:
          SCSI_Read12_Cmd();
          break;
        case SCSI_READ16:
          SCSI_Read16_Cmd();
          break;
        case SCSI_READ_CAPACITY16:
          SCSI_READ_CAPACITY16_Cmd();
          break;
        case SCSI_WRITE6:
          SCSI_Write6_Cmd();
          break;
        case SCSI_WRITE12:
          SCSI_Write12_Cmd();
          break;
        case SCSI_WRITE16:
          SCSI_Write16_Cmd();
          break;
        case SCSI_VERIFY12:
          SCSI_Verify12_Cmd();
          break;
        case SCSI_VERIFY16:
          SCSI_Verify16_Cmd();
          break;

        default:
        {
          Bot_Abort(BOTH_DIR);
          Set_Scsi_Sense_Data(ILLEGAL_REQUEST, INVALID_COMMAND);
          Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
        }
      }
    }
  }
  else
  {
    // Invalid CBW
    Bot_Abort(BOTH_DIR);
    Set_Scsi_Sense_Data(ILLEGAL_REQUEST, INVALID_COMMAND);
    Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
  }
}
/*******************************************************************************
  Transfer_Data_Request: Send the request response to the PC HOST.
  Input          : u8* Data_Address : point to the data to transfer.
                   u16 Data_Length : the nember of Bytes to transfer.
*******************************************************************************/
void Transfer_Data_Request(u8* Data_Pointer, u16 Data_Len)
{
  UserToPMABufferCopy(Data_Pointer, ENDP1_TXADDR, Data_Len);
  SetEPTxCount(ENDP1, Data_Len);
  SetEPTxStatus(ENDP1, EP_TX_VALID);
  Bot_State = BOT_DATA_IN_LAST;
  CSW.dDataResidue -= Data_Len;
  CSW.bStatus = CSW_CMD_PASSED;
}
/*******************************************************************************
  Set_CSW: Set the SCW with the needed fields.
  Input          : u8 CSW_Status this filed can be CSW_CMD_PASSED,CSW_CMD_FAILED,
                   or CSW_PHASE_ERROR.
*******************************************************************************/
void Set_CSW (u8 CSW_Status, u8 Send_Permission)
{
  CSW.dSignature = BOT_CSW_SIGNATURE;
  CSW.bStatus = CSW_Status;
  UserToPMABufferCopy(((u8 *)& CSW), ENDP1_TXADDR, CSW_DATA_LENGTH);
  SetEPTxCount(ENDP1, CSW_DATA_LENGTH);
  Bot_State = BOT_ERROR;
  if (Send_Permission){
    Bot_State = BOT_CSW_Send;
    SetEPTxStatus(ENDP1, EP_TX_VALID);
  }
}
/*******************************************************************************
  Bot_Abort: Stall the needed Endpoint according to the selected direction.
  Input          : Endpoint direction IN, OUT or both directions
*******************************************************************************/
void Bot_Abort(u8 Direction)
{
  switch (Direction){
    case DIR_IN :
      SetEPTxStatus(ENDP1, EP_TX_STALL);
      break;
    case DIR_OUT :
      SetEPRxStatus(ENDP2, EP_RX_STALL);
      break;
    case BOTH_DIR :
      SetEPTxStatus(ENDP1, EP_TX_STALL);
      SetEPRxStatus(ENDP2, EP_RX_STALL);
      break;
    default:
      break;
  }
}
/*********************************  END OF FILE  ******************************/
