/******************** (C) COPYRIGHT 2015 e-Design Co., Ltd. ********************
 File Name : USB_desc.c
 Version   : STM32_USB Disk Ver 3.4       Author : MCD Application Team & bure
*******************************************************************************/
#include "USB_desc.h"

const u8 MASS_DeviceDescriptor[MASS_SIZ_DEVICE_DESC] ={
    0x12,   /* bLength  */
    0x01,   /* bDescriptorType */
    0x00,   /* bcdUSB, version 2.00 */
    0x02,
    0x00,   /* bDeviceClass : each interface define the device class */
    0x00,   /* bDeviceSubClass */
    0x00,   /* bDeviceProtocol */
    0x40,   /* bMaxPacketSize0 0x40 = 64 */
    0x83,   /* idVendor     (0483) */
    0x04,
    0x20,   /* idProduct */
    0x57,
    0x00,   /* bcdDevice 2.00*/
    0x02,
    1,              /* index of string Manufacturer  */
    /**/
    2,              /* index of string descriptor of product*/
    /* */
    3,              /* */
    /* */
    /* */
    0x01    /*bNumConfigurations */
  };
const u8 MASS_ConfigDescriptor[MASS_SIZ_CONFIG_DESC] ={
    0x09,   /* bLength: Configuation Descriptor size */
    0x02,   /* bDescriptorType: Configuration */
    MASS_SIZ_CONFIG_DESC,
    0x00,
    0x01,   /* bNumInterfaces: 1 interface */
    0x01,   /* bConfigurationValue: */
    /*      Configuration value */
    0x00,   /* iConfiguration: */
    /*      Index of string descriptor */
    /*      describing the configuration */
    0xC0,   /* bmAttributes: */
    /*      bus powered */
    0x32,   /* MaxPower 100 mA */

    /******************** Descriptor of Mass Storage interface ********************/
    /* 09 */
    0x09,   /* bLength: Interface Descriptor size */
    0x04,   /* bDescriptorType: */
    /*      Interface descriptor type */
    0x00,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x02,   /* bNumEndpoints*/
    0x08,   /* bInterfaceClass: MASS STORAGE Class */
    0x06,   /* bInterfaceSubClass : SCSI transparent*/
    0x50,   /* nInterfaceProtocol */
    4,          /* iInterface: */
    /* 18 */
    0x07,   /*Endpoint descriptor length = 7*/
    0x05,   /*Endpoint descriptor type */
    0x81,   /*Endpoint address (IN, address 1) */
    0x02,   /*Bulk endpoint type */
    0x40,   /*Maximum packet size (64 bytes) */
    0x00,
    0x00,   /*Polling interval in milliseconds */
    /* 25 */
    0x07,   /*Endpoint descriptor length = 7 */
    0x05,   /*Endpoint descriptor type */
    0x02,   /*Endpoint address (OUT, address 2) */
    0x02,   /*Bulk endpoint type */
    0x40,   /*Maximum packet size (64 bytes) */
    0x00,
    0x00     /*Polling interval in milliseconds*/
    /*32*/
  };
const u8 MASS_StringLangID[MASS_SIZ_STRING_LANGID] ={
    MASS_SIZ_STRING_LANGID,
    0x03,
    0x09,
    0x04
  };      // LangID = 0x0409: U.S. English //
const u8 MASS_StringVendor[MASS_SIZ_STRING_VENDOR] ={
    MASS_SIZ_STRING_VENDOR, // Size of manufaturer string //
    0x03,           // bDescriptorType = String descriptor //
    // Manufacturer: "STMicroelectronics" //
    'S', 0, 'T', 0, 'M', 0, 'i', 0, 'c', 0, 'r', 0, 'o', 0, 'e', 0,
    'l', 0, 'e', 0, 'c', 0, 't', 0, 'r', 0, 'o', 0, 'n', 0, 'i', 0,
    'c', 0, 's', 0
  };
const u8 MASS_StringProduct[MASS_SIZ_STRING_PRODUCT] ={
    MASS_SIZ_STRING_PRODUCT,
    0x03,
    // Product name: "STM32F10x:USB Mass Storage" //
    'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0, ' ', 0, 'M', 0, 'a', 0, 's', 0,
    's', 0, ' ', 0, 'S', 0, 't', 0, 'o', 0, 'r', 0, 'a', 0, 'g', 0, 'e', 0
  };
u8 MASS_StringSerial[MASS_SIZ_STRING_SERIAL] ={
    MASS_SIZ_STRING_SERIAL,
    0x03,
    // Serial number//
    'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0, '1', 0, '0', 0
  };
const u8 MASS_StringInterface[MASS_SIZ_STRING_INTERFACE] ={
    MASS_SIZ_STRING_INTERFACE,
    0x03,
    // Interface 0: "ST Mass" //
    'S', 0, 'T', 0, ' ', 0, 'M', 0, 'a', 0, 's', 0, 's', 0
  };
/*********************************  END OF FILE  ******************************/
