/**
 * @file
 * @brief USB Communications Device Class (CDC) public header
 *
 * Header follows the Class Definitions for
 * Communications Devices Specification (CDC120-20101103-track.pdf),
 * PSTN Devices Specification (PSTN120.pdf) and
 * Ethernet Control Model Devices Specification (ECM120.pdf).
 * Header is limited to ACM and ECM Subclasses.
 */

#ifndef _USBD_CDC_H
#define _USBD_CDC_H

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
 *      Definitions  based on usbcdc11.pdf (www.usb.org)
 *----------------------------------------------------------------------------*/
/* Communication device class specification version 1.10 */
#define CDC_V1_10 0x0110U
// Communication device class specification version 1.2
#define CDC_V1_2_0 0x0120U

/* Communication interface class code */
/* (usbcdc11.pdf, 4.2, Table 15) */
#define CDC_COMMUNICATION_INTERFACE_CLASS 0x02U

/* Communication interface class subclass codes */
/* (usbcdc11.pdf, 4.3, Table 16) */
#define CDC_DIRECT_LINE_CONTROL_MODEL         0x01U
#define CDC_ABSTRACT_CONTROL_MODEL            0x02U
#define CDC_TELEPHONE_CONTROL_MODEL           0x03U
#define CDC_MULTI_CHANNEL_CONTROL_MODEL       0x04U
#define CDC_CAPI_CONTROL_MODEL                0x05U
#define CDC_ETHERNET_NETWORKING_CONTROL_MODEL 0x06U
#define CDC_ATM_NETWORKING_CONTROL_MODEL      0x07U
#define CDC_WIRELESS_HANDSET_CONTROL_MODEL    0x08U
#define CDC_DEVICE_MANAGEMENT                 0x09U
#define CDC_MOBILE_DIRECT_LINE_MODEL          0x0AU
#define CDC_OBEX                              0x0BU
#define CDC_ETHERNET_EMULATION_MODEL          0x0CU
#define CDC_NETWORK_CONTROL_MODEL             0x0DU

/* Communication interface class control protocol codes */
/* (usbcdc11.pdf, 4.4, Table 17) */
#define CDC_COMMON_PROTOCOL_NONE                            0x00U
#define CDC_COMMON_PROTOCOL_AT_COMMANDS                     0x01U
#define CDC_COMMON_PROTOCOL_AT_COMMANDS_PCCA_101            0x02U
#define CDC_COMMON_PROTOCOL_AT_COMMANDS_PCCA_101_AND_ANNEXO 0x03U
#define CDC_COMMON_PROTOCOL_AT_COMMANDS_GSM_707             0x04U
#define CDC_COMMON_PROTOCOL_AT_COMMANDS_3GPP_27007          0x05U
#define CDC_COMMON_PROTOCOL_AT_COMMANDS_CDMA                0x06U
#define CDC_COMMON_PROTOCOL_ETHERNET_EMULATION_MODEL        0x07U
// NCM Communication Interface Protocol Codes
// (usbncm10.pdf, 4.2, Table 4-2)
#define CDC_NCM_PROTOCOL_NONE 0x00U
#define CDC_NCM_PROTOCOL_OEM  0xFEU

/* Data interface class code */
/* (usbcdc11.pdf, 4.5, Table 18) */
#define CDC_DATA_INTERFACE_CLASS 0x0A

/* Data interface class protocol codes */
/* (usbcdc11.pdf, 4.7, Table 19) */
#define CDC_DATA_PROTOCOL_ISDN_BRI            0x30
#define CDC_DATA_PROTOCOL_HDLC                0x31
#define CDC_DATA_PROTOCOL_TRANSPARENT         0x32
#define CDC_DATA_PROTOCOL_Q921_MANAGEMENT     0x50
#define CDC_DATA_PROTOCOL_Q921_DATA_LINK      0x51
#define CDC_DATA_PROTOCOL_Q921_MULTIPLEXOR    0x52
#define CDC_DATA_PROTOCOL_V42                 0x90
#define CDC_DATA_PROTOCOL_EURO_ISDN           0x91
#define CDC_DATA_PROTOCOL_V24_RATE_ADAPTATION 0x92
#define CDC_DATA_PROTOCOL_CAPI                0x93
#define CDC_DATA_PROTOCOL_HOST_BASED_DRIVER   0xFD
#define CDC_DATA_PROTOCOL_DESCRIBED_IN_PUFD   0xFE

/* Type values for bDescriptorType field of functional descriptors */
/* (usbcdc11.pdf, 5.2.3, Table 24) */
#define CDC_CS_INTERFACE 0x24
#define CDC_CS_ENDPOINT  0x25

/* Type values for bDescriptorSubtype field of functional descriptors */
/* (usbcdc11.pdf, 5.2.3, Table 25) */
#define CDC_FUNC_DESC_HEADER                          0x00
#define CDC_FUNC_DESC_CALL_MANAGEMENT                 0x01
#define CDC_FUNC_DESC_ABSTRACT_CONTROL_MANAGEMENT     0x02
#define CDC_FUNC_DESC_DIRECT_LINE_MANAGEMENT          0x03
#define CDC_FUNC_DESC_TELEPHONE_RINGER                0x04
#define CDC_FUNC_DESC_REPORTING_CAPABILITIES          0x05
#define CDC_FUNC_DESC_UNION                           0x06
#define CDC_FUNC_DESC_COUNTRY_SELECTION               0x07
#define CDC_FUNC_DESC_TELEPHONE_OPERATIONAL_MODES     0x08
#define CDC_FUNC_DESC_USB_TERMINAL                    0x09
#define CDC_FUNC_DESC_NETWORK_CHANNEL                 0x0A
#define CDC_FUNC_DESC_PROTOCOL_UNIT                   0x0B
#define CDC_FUNC_DESC_EXTENSION_UNIT                  0x0C
#define CDC_FUNC_DESC_MULTI_CHANNEL_MANAGEMENT        0x0D
#define CDC_FUNC_DESC_CAPI_CONTROL_MANAGEMENT         0x0E
#define CDC_FUNC_DESC_ETHERNET_NETWORKING             0x0F
#define CDC_FUNC_DESC_ATM_NETWORKING                  0x10
#define CDC_FUNC_DESC_WIRELESS_HANDSET_CONTROL_MODEL  0x11
#define CDC_FUNC_DESC_MOBILE_DIRECT_LINE_MODEL        0x12
#define CDC_FUNC_DESC_MOBILE_DIRECT_LINE_MODEL_DETAIL 0x13
#define CDC_FUNC_DESC_DEVICE_MANAGEMENT_MODEL         0x14
#define CDC_FUNC_DESC_OBEX                            0x15
#define CDC_FUNC_DESC_COMMAND_SET                     0x16
#define CDC_FUNC_DESC_COMMAND_SET_DETAIL              0x17
#define CDC_FUNC_DESC_TELEPHONE_CONTROL_MODEL         0x18
#define CDC_FUNC_DESC_OBEX_SERVICE_IDENTIFIER         0x19

/* CDC class-specific request codes */
/* (usbcdc11.pdf, 6.2, Table 46) */
/* see Table 45 for info about the specific requests. */
#define CDC_REQUEST_SEND_ENCAPSULATED_COMMAND      0x00
#define CDC_REQUEST_GET_ENCAPSULATED_RESPONSE      0x01
#define CDC_REQUEST_SET_COMM_FEATURE               0x02
#define CDC_REQUEST_GET_COMM_FEATURE               0x03
#define CDC_REQUEST_CLEAR_COMM_FEATURE             0x04
#define CDC_REQUEST_SET_AUX_LINE_STATE             0x10
#define CDC_REQUEST_SET_HOOK_STATE                 0x11
#define CDC_REQUEST_PULSE_SETUP                    0x12
#define CDC_REQUEST_SEND_PULSE                     0x13
#define CDC_REQUEST_SET_PULSE_TIME                 0x14
#define CDC_REQUEST_RING_AUX_JACK                  0x15
#define CDC_REQUEST_SET_LINE_CODING                0x20
#define CDC_REQUEST_GET_LINE_CODING                0x21
#define CDC_REQUEST_SET_CONTROL_LINE_STATE         0x22
#define CDC_REQUEST_SEND_BREAK                     0x23
#define CDC_REQUEST_SET_RINGER_PARMS               0x30
#define CDC_REQUEST_GET_RINGER_PARMS               0x31
#define CDC_REQUEST_SET_OPERATION_PARMS            0x32
#define CDC_REQUEST_GET_OPERATION_PARMS            0x33
#define CDC_REQUEST_SET_LINE_PARMS                 0x34
#define CDC_REQUEST_GET_LINE_PARMS                 0x35
#define CDC_REQUEST_DIAL_DIGITS                    0x36
#define CDC_REQUEST_SET_UNIT_PARAMETER             0x37
#define CDC_REQUEST_GET_UNIT_PARAMETER             0x38
#define CDC_REQUEST_CLEAR_UNIT_PARAMETER           0x39
#define CDC_REQUEST_GET_PROFILE                    0x3A
#define CDC_REQUEST_SET_ETHERNET_MULTICAST_FILTERS 0x40
#define CDC_REQUEST_SET_ETHERNET_PMP_FILTER        0x41
#define CDC_REQUEST_GET_ETHERNET_PMP_FILTER        0x42
#define CDC_REQUEST_SET_ETHERNET_PACKET_FILTER     0x43
#define CDC_REQUEST_GET_ETHERNET_STATISTIC         0x44
#define CDC_REQUEST_SET_ATM_DATA_FORMAT            0x50
#define CDC_REQUEST_GET_ATM_DEVICE_STATISTICS      0x51
#define CDC_REQUEST_SET_ATM_DEFAULT_VC             0x52
#define CDC_REQUEST_GET_ATM_VC_STATISTICS          0x53

/* Communication feature selector codes */
/* (usbcdc11.pdf, 6.2.2..6.2.4, Table 47) */
#define CDC_ABSTRACT_STATE  0x01
#define CDC_COUNTRY_SETTING 0x02

/** Control Signal Bitmap Values for SetControlLineState */
#define SET_CONTROL_LINE_STATE_RTS 0x02
#define SET_CONTROL_LINE_STATE_DTR 0x01

/* Feature Status returned for ABSTRACT_STATE Selector */
/* (usbcdc11.pdf, 6.2.3, Table 48) */
#define CDC_IDLE_SETTING          (1 << 0)
#define CDC_DATA_MULTPLEXED_STATE (1 << 1)

/* Control signal bitmap values for the SetControlLineState request */
/* (usbcdc11.pdf, 6.2.14, Table 51) */
#define CDC_DTE_PRESENT      (1 << 0)
#define CDC_ACTIVATE_CARRIER (1 << 1)

/* CDC class-specific notification codes */
/* (usbcdc11.pdf, 6.3, Table 68) */
/* see Table 67 for Info about class-specific notifications */
#define CDC_NOTIFICATION_NETWORK_CONNECTION 0x00
#define CDC_RESPONSE_AVAILABLE              0x01
#define CDC_AUX_JACK_HOOK_STATE             0x08
#define CDC_RING_DETECT                     0x09
#define CDC_NOTIFICATION_SERIAL_STATE       0x20
#define CDC_CALL_STATE_CHANGE               0x28
#define CDC_LINE_STATE_CHANGE               0x29
#define CDC_CONNECTION_SPEED_CHANGE         0x2A

/* UART state bitmap values (Serial state notification). */
/* (usbcdc11.pdf, 6.3.5, Table 69) */
#define CDC_SERIAL_STATE_OVERRUN        (1 << 6) /* receive data overrun error has occurred */
#define CDC_SERIAL_STATE_OVERRUN_Pos    (6)
#define CDC_SERIAL_STATE_OVERRUN_Msk    (1 << CDC_SERIAL_STATE_OVERRUN_Pos)
#define CDC_SERIAL_STATE_PARITY         (1 << 5) /* parity error has occurred */
#define CDC_SERIAL_STATE_PARITY_Pos     (5)
#define CDC_SERIAL_STATE_PARITY_Msk     (1 << CDC_SERIAL_STATE_PARITY_Pos)
#define CDC_SERIAL_STATE_FRAMING        (1 << 4) /* framing error has occurred */
#define CDC_SERIAL_STATE_FRAMING_Pos    (4)
#define CDC_SERIAL_STATE_FRAMING_Msk    (1 << CDC_SERIAL_STATE_FRAMING_Pos)
#define CDC_SERIAL_STATE_RING           (1 << 3) /* state of ring signal detection */
#define CDC_SERIAL_STATE_RING_Pos       (3)
#define CDC_SERIAL_STATE_RING_Msk       (1 << CDC_SERIAL_STATE_RING_Pos)
#define CDC_SERIAL_STATE_BREAK          (1 << 2) /* state of break detection */
#define CDC_SERIAL_STATE_BREAK_Pos      (2)
#define CDC_SERIAL_STATE_BREAK_Msk      (1 << CDC_SERIAL_STATE_BREAK_Pos)
#define CDC_SERIAL_STATE_TX_CARRIER     (1 << 1) /* state of transmission carrier */
#define CDC_SERIAL_STATE_TX_CARRIER_Pos (1)
#define CDC_SERIAL_STATE_TX_CARRIER_Msk (1 << CDC_SERIAL_STATE_TX_CARRIER_Pos)
#define CDC_SERIAL_STATE_RX_CARRIER     (1 << 0) /* state of receiver carrier */
#define CDC_SERIAL_STATE_RX_CARRIER_Pos (0)
#define CDC_SERIAL_STATE_RX_CARRIER_Msk (1 << CDC_SERIAL_STATE_RX_CARRIER_Pos)

/*------------------------------------------------------------------------------
 *      Structures  based on usbcdc11.pdf (www.usb.org)
 *----------------------------------------------------------------------------*/

/* Header functional descriptor */
/* (usbcdc11.pdf, 5.2.3.1) */
/* This header must precede any list of class-specific descriptors. */
struct cdc_header_descriptor {
    uint8_t bFunctionLength;    /* size of this descriptor in bytes */
    uint8_t bDescriptorType;    /* CS_INTERFACE descriptor type */
    uint8_t bDescriptorSubtype; /* Header functional descriptor subtype */
    uint16_t bcdCDC;            /* USB CDC specification release version */
} __packed;

/* Call management functional descriptor */
/* (usbcdc11.pdf, 5.2.3.2) */
/* Describes the processing of calls for the communication class interface. */
struct cdc_call_management_descriptor {
    uint8_t bFunctionLength;    /* size of this descriptor in bytes */
    uint8_t bDescriptorType;    /* CS_INTERFACE descriptor type */
    uint8_t bDescriptorSubtype; /* call management functional descriptor subtype */
    uint8_t bmCapabilities;     /* capabilities that this configuration supports */
    uint8_t bDataInterface;     /* interface number of the data class interface used for call management (optional) */
} __packed;

/* Abstract control management functional descriptor */
/* (usbcdc11.pdf, 5.2.3.3) */
/* Describes the command supported by the communication interface class with the Abstract Control Model subclass code. */
struct cdc_abstract_control_management_descriptor {
    uint8_t bFunctionLength;    /* size of this descriptor in bytes */
    uint8_t bDescriptorType;    /* CS_INTERFACE descriptor type */
    uint8_t bDescriptorSubtype; /* abstract control management functional descriptor subtype */
    uint8_t bmCapabilities;     /* capabilities supported by this configuration */
} __packed;

/* Union functional descriptors */
/* (usbcdc11.pdf, 5.2.3.8) */
/* Describes the relationship between a group of interfaces that can be considered to form a functional unit. */
struct cdc_union_descriptor {
    uint8_t bFunctionLength;    /* size of this descriptor in bytes */
    uint8_t bDescriptorType;    /* CS_INTERFACE descriptor type */
    uint8_t bDescriptorSubtype; /* union functional descriptor subtype */
    uint8_t bMasterInterface;   /* interface number designated as master */
} __packed;

/* Union functional descriptors with one slave interface */
/* (usbcdc11.pdf, 5.2.3.8) */
struct cdc_union_1slave_descriptor {
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bControlInterface;
    uint8_t bSubordinateInterface0;
} __packed;

/* Line coding structure for GET_LINE_CODING / SET_LINE_CODING class requests*/
/* Format of the data returned when a GetLineCoding request is received */
/* (usbcdc11.pdf, 6.2.13) */
struct cdc_line_coding {
    uint32_t dwDTERate;  /* Data terminal rate in bits per second */
    uint8_t bCharFormat; /* Number of stop bits */
    uint8_t bParityType; /* Parity bit type */
    uint8_t bDataBits;   /* Number of data bits */
} __packed;

/** Data structure for the notification about SerialState */
struct cdc_acm_notification {
    uint8_t bmRequestType;
    uint8_t bNotificationType;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
    uint16_t data;
} __packed;

/** Ethernet Networking Functional Descriptor */
struct cdc_ecm_descriptor {
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t iMACAddress;
    uint32_t bmEthernetStatistics;
    uint16_t wMaxSegmentSize;
    uint16_t wNumberMCFilters;
    uint8_t bNumberPowerFilters;
} __packed;

/*Length of template descriptor: 66 bytes*/
#define CDC_ACM_DESCRIPTOR_LEN (8 + 9 + 5 + 5 + 4 + 5 + 7 + 9 + 7 + 7)
// clang-format off
#ifndef CONFIG_USB_HS
#define CDC_ACM_DESCRIPTOR_INIT(bFirstInterface, int_ep, out_ep, in_ep, str_idx)               \
    /* Interface Associate */                                                                  \
    0x08,                                                  /* bLength */                       \
    USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION,             /* bDescriptorType */               \
    bFirstInterface,                                       /* bFirstInterface */               \
    0x02,                                                  /* bInterfaceCount */               \
    USB_DEVICE_CLASS_CDC,                                  /* bFunctionClass */                \
    CDC_ABSTRACT_CONTROL_MODEL,                            /* bFunctionSubClass */             \
    CDC_COMMON_PROTOCOL_AT_COMMANDS,                       /* bFunctionProtocol */             \
    0x00,                                                  /* iFunction */                     \
    0x09,                                                  /* bLength */                       \
    USB_DESCRIPTOR_TYPE_INTERFACE,                         /* bDescriptorType */               \
    bFirstInterface,                                       /* bInterfaceNumber */              \
    0x00,                                                  /* bAlternateSetting */             \
    0x01,                                                  /* bNumEndpoints */                 \
    USB_DEVICE_CLASS_CDC,                                  /* bInterfaceClass */               \
    CDC_ABSTRACT_CONTROL_MODEL,                            /* bInterfaceSubClass */            \
    CDC_COMMON_PROTOCOL_AT_COMMANDS,                       /* bInterfaceProtocol */            \
    str_idx,                                               /* iInterface */                    \
    0x05,                                                  /* bLength */                       \
    CDC_CS_INTERFACE,                                      /* bDescriptorType */               \
    CDC_FUNC_DESC_HEADER,                                  /* bDescriptorSubtype */            \
    WBVAL(CDC_V1_10),                                      /* bcdCDC */                        \
    0x05,                                                  /* bLength */                       \
    CDC_CS_INTERFACE,                                      /* bDescriptorType */               \
    CDC_FUNC_DESC_CALL_MANAGEMENT,                         /* bDescriptorSubtype */            \
    bFirstInterface,                                       /* bmCapabilities */                \
    (uint8_t)(bFirstInterface + 1),                        /* bDataInterface */                \
    0x04,                                                  /* bLength */                       \
    CDC_CS_INTERFACE,                                      /* bDescriptorType */               \
    CDC_FUNC_DESC_ABSTRACT_CONTROL_MANAGEMENT,             /* bDescriptorSubtype */            \
    0x02,                                                  /* bmCapabilities */                \
    0x05,                                                  /* bLength */                       \
    CDC_CS_INTERFACE,                                      /* bDescriptorType */               \
    CDC_FUNC_DESC_UNION,                                   /* bDescriptorSubtype */            \
    bFirstInterface,                                       /* bMasterInterface */              \
    (uint8_t)(bFirstInterface + 1),                        /* bSlaveInterface0 */              \
    0x07,                                                  /* bLength */                       \
    USB_DESCRIPTOR_TYPE_ENDPOINT,                          /* bDescriptorType */               \
    int_ep,                                                /* bEndpointAddress */              \
    0x03,                                                  /* bmAttributes */                  \
    0x40, 0x00,                                            /* wMaxPacketSize */                \
    0x00,                                                  /* bInterval */                     \
    0x09,                                                  /* bLength */                       \
    USB_DESCRIPTOR_TYPE_INTERFACE,                         /* bDescriptorType */               \
    (uint8_t)(bFirstInterface + 1),                        /* bInterfaceNumber */              \
    0x00,                                                  /* bAlternateSetting */             \
    0x02,                                                  /* bNumEndpoints */                 \
    CDC_DATA_INTERFACE_CLASS,                              /* bInterfaceClass */               \
    0x00,                                                  /* bInterfaceSubClass */            \
    0x00,                                                  /* bInterfaceProtocol */            \
    0x00,                                                  /* iInterface */                    \
    0x07,                                                  /* bLength */                       \
    USB_DESCRIPTOR_TYPE_ENDPOINT,                          /* bDescriptorType */               \
    out_ep,                                                /* bEndpointAddress */              \
    0x02,                                                  /* bmAttributes */                  \
    0x40, 0x00,                                            /* wMaxPacketSize */                \
    0x00,                                                  /* bInterval */                     \
    0x07,                                                  /* bLength */                       \
    USB_DESCRIPTOR_TYPE_ENDPOINT,                          /* bDescriptorType */               \
    in_ep,                                                 /* bEndpointAddress */              \
    0x02,                                                  /* bmAttributes */                  \
    0x40, 0x00,                                            /* wMaxPacketSize */                \
    0x00                                                   /* bInterval */
#else
#define CDC_ACM_DESCRIPTOR_INIT(bFirstInterface, int_ep, out_ep, in_ep, str_idx)               \
    /* Interface Associate */                                                                  \
    0x08,                                                  /* bLength */                       \
    USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION,             /* bDescriptorType */               \
    bFirstInterface,                                       /* bFirstInterface */               \
    0x02,                                                  /* bInterfaceCount */               \
    USB_DEVICE_CLASS_CDC,                                  /* bFunctionClass */                \
    CDC_ABSTRACT_CONTROL_MODEL,                            /* bFunctionSubClass */             \
    CDC_COMMON_PROTOCOL_AT_COMMANDS,                       /* bFunctionProtocol */             \
    0x00,                                                  /* iFunction */                     \
    0x09,                                                  /* bLength */                       \
    USB_DESCRIPTOR_TYPE_INTERFACE,                         /* bDescriptorType */               \
    bFirstInterface,                                       /* bInterfaceNumber */              \
    0x00,                                                  /* bAlternateSetting */             \
    0x01,                                                  /* bNumEndpoints */                 \
    USB_DEVICE_CLASS_CDC,                                  /* bInterfaceClass */               \
    CDC_ABSTRACT_CONTROL_MODEL,                            /* bInterfaceSubClass */            \
    CDC_COMMON_PROTOCOL_AT_COMMANDS,                       /* bInterfaceProtocol */            \
    str_idx,                                               /* iInterface */                    \
    0x05,                                                  /* bLength */                       \
    CDC_CS_INTERFACE,                                      /* bDescriptorType */               \
    CDC_FUNC_DESC_HEADER,                                  /* bDescriptorSubtype */            \
    WBVAL(CDC_V1_10),                                      /* bcdCDC */                        \
    0x05,                                                  /* bLength */                       \
    CDC_CS_INTERFACE,                                      /* bDescriptorType */               \
    CDC_FUNC_DESC_CALL_MANAGEMENT,                         /* bDescriptorSubtype */            \
    bFirstInterface,                                       /* bmCapabilities */                \
    (uint8_t)(bFirstInterface + 1),                        /* bDataInterface */                \
    0x04,                                                  /* bLength */                       \
    CDC_CS_INTERFACE,                                      /* bDescriptorType */               \
    CDC_FUNC_DESC_ABSTRACT_CONTROL_MANAGEMENT,             /* bDescriptorSubtype */            \
    0x02,                                                  /* bmCapabilities */                \
    0x05,                                                  /* bLength */                       \
    CDC_CS_INTERFACE,                                      /* bDescriptorType */               \
    CDC_FUNC_DESC_UNION,                                   /* bDescriptorSubtype */            \
    bFirstInterface,                                       /* bMasterInterface */              \
    (uint8_t)(bFirstInterface + 1),                        /* bSlaveInterface0 */              \
    0x07,                                                  /* bLength */                       \
    USB_DESCRIPTOR_TYPE_ENDPOINT,                          /* bDescriptorType */               \
    int_ep,                                                /* bEndpointAddress */              \
    0x03,                                                  /* bmAttributes */                  \
    0x00, 0x02,                                            /* wMaxPacketSize */                \
    0x00,                                                  /* bInterval */                     \
    0x09,                                                  /* bLength */                       \
    USB_DESCRIPTOR_TYPE_INTERFACE,                         /* bDescriptorType */               \
    (uint8_t)(bFirstInterface + 1),                        /* bInterfaceNumber */              \
    0x00,                                                  /* bAlternateSetting */             \
    0x02,                                                  /* bNumEndpoints */                 \
    CDC_DATA_INTERFACE_CLASS,                              /* bInterfaceClass */               \
    0x00,                                                  /* bInterfaceSubClass */            \
    0x00,                                                  /* bInterfaceProtocol */            \
    0x00,                                                  /* iInterface */                    \
    0x07,                                                  /* bLength */                       \
    USB_DESCRIPTOR_TYPE_ENDPOINT,                          /* bDescriptorType */               \
    out_ep,                                                /* bEndpointAddress */              \
    0x02,                                                  /* bmAttributes */                  \
    0x00, 0x02,                                            /* wMaxPacketSize */                \
    0x00,                                                  /* bInterval */                     \
    0x07,                                                  /* bLength */                       \
    USB_DESCRIPTOR_TYPE_ENDPOINT,                          /* bDescriptorType */               \
    in_ep,                                                 /* bEndpointAddress */              \
    0x02,                                                  /* bmAttributes */                  \
    0x00, 0x02,                                            /* wMaxPacketSize */                \
    0x00                                                   /* bInterval */
#endif
// clang-format on

void usbd_cdc_add_acm_interface(usbd_class_t *class, usbd_interface_t *intf);

void usbd_cdc_acm_set_line_coding(uint32_t baudrate, uint8_t databits, uint8_t parity, uint8_t stopbits);
void usbd_cdc_acm_set_dtr(bool dtr);
void usbd_cdc_acm_set_rts(bool rts);

#ifdef __cplusplus
}
#endif

#endif /* USB_CDC_H_ */
