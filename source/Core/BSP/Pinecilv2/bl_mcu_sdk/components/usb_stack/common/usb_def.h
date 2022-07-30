#ifndef USB_REQUEST_H
#define USB_REQUEST_H

/* Useful define */
#define USB_1_1 0x0110
#define USB_2_0 0x0200
/* Set USB version to 2.1 so that the host will request the BOS descriptor */
#define USB_2_1 0x0210

// USB Speed
#define USB_SPEED_LOW  0U
#define USB_SPEED_FULL 1U
#define USB_SPEED_HIGH 2U

// USB PID Types
#define USB_PID_RESERVED 0U
#define USB_PID_OUT      1U
#define USB_PID_ACK      2U
#define USB_PID_DATA0    3U
#define USB_PID_PING     4U
#define USB_PID_SOF      5U
#define USB_PID_DATA2    7U
#define USB_PID_NYET     6U
#define USB_PID_SPLIT    8U
#define USB_PID_IN       9U
#define USB_PID_NAK      10U
#define USB_PID_DATA1    11U
#define USB_PID_PRE      12U
#define USB_PID_ERR      12U
#define USB_PID_SETUP    13U
#define USB_PID_STALL    14U
#define USB_PID_MDATA    15U

// bmRequestType.Dir
#define USB_REQUEST_HOST_TO_DEVICE 0U
#define USB_REQUEST_DEVICE_TO_HOST 1U

// bmRequestType.Type
#define USB_REQUEST_STANDARD 0U
#define USB_REQUEST_CLASS    1U
#define USB_REQUEST_VENDOR   2U
#define USB_REQUEST_RESERVED 3U

// bmRequestType.Recipient
#define USB_REQUEST_TO_DEVICE    0U
#define USB_REQUEST_TO_INTERFACE 1U
#define USB_REQUEST_TO_ENDPOINT  2U
#define USB_REQUEST_TO_OTHER     3U

/* USB Standard Request Codes */
#define USB_REQUEST_GET_STATUS          0x00
#define USB_REQUEST_CLEAR_FEATURE       0x01
#define USB_REQUEST_SET_FEATURE         0x03
#define USB_REQUEST_SET_ADDRESS         0x05
#define USB_REQUEST_GET_DESCRIPTOR      0x06
#define USB_REQUEST_SET_DESCRIPTOR      0x07
#define USB_REQUEST_GET_CONFIGURATION   0x08
#define USB_REQUEST_SET_CONFIGURATION   0x09
#define USB_REQUEST_GET_INTERFACE       0x0A
#define USB_REQUEST_SET_INTERFACE       0x0B
#define USB_REQUEST_SYNCH_FRAME         0x0C
#define USB_REQUEST_SET_ENCRYPTION      0x0D
#define USB_REQUEST_GET_ENCRYPTION      0x0E
#define USB_REQUEST_RPIPE_ABORT         0x0E
#define USB_REQUEST_SET_HANDSHAKE       0x0F
#define USB_REQUEST_RPIPE_RESET         0x0F
#define USB_REQUEST_GET_HANDSHAKE       0x10
#define USB_REQUEST_SET_CONNECTION      0x11
#define USB_REQUEST_SET_SECURITY_DATA   0x12
#define USB_REQUEST_GET_SECURITY_DATA   0x13
#define USB_REQUEST_SET_WUSB_DATA       0x14
#define USB_REQUEST_LOOPBACK_DATA_WRITE 0x15
#define USB_REQUEST_LOOPBACK_DATA_READ  0x16
#define USB_REQUEST_SET_INTERFACE_DS    0x17

/* USB GET_STATUS Bit Values */
#define USB_GETSTATUS_SELF_POWERED   0x01
#define USB_GETSTATUS_REMOTE_WAKEUP  0x02
#define USB_GETSTATUS_ENDPOINT_STALL 0x01

/* USB Standard Feature selectors */
#define USB_FEATURE_ENDPOINT_STALL 0
#define USB_FEATURE_REMOTE_WAKEUP  1
#define USB_FEATURE_TEST_MODE      2

/* Descriptor size in bytes */
#define USB_DEVICE_DESC_SIZE          0x12
#define USB_CONFIGURATION_DESC_SIZE   0x09
#define USB_INTERFACE_DESC_SIZE       0x09
#define USB_ENDPOINT_DESC_SIZE        0x07
#define USB_LANGID_STRING_DESC_SIZE   0x04
#define USB_OTHER_SPEED_DESC_SIZE     0x09
#define USB_DEVICE_QUAL_DESC_SIZE     0x0A
#define USB_INTERFACE_ASSOC_DESC_SIZE 0x08
#define USB_FUNCTION_DESC_SIZE        0x03
#define USB_OTG_DESC_SIZE             0x03

/* USB Descriptor Types */
#define USB_DESCRIPTOR_TYPE_DEVICE                0x01U
#define USB_DESCRIPTOR_TYPE_CONFIGURATION         0x02U
#define USB_DESCRIPTOR_TYPE_STRING                0x03U
#define USB_DESCRIPTOR_TYPE_INTERFACE             0x04U
#define USB_DESCRIPTOR_TYPE_ENDPOINT              0x05U
#define USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER      0x06U
#define USB_DESCRIPTOR_TYPE_OTHER_SPEED           0x07U
#define USB_DESCRIPTOR_TYPE_INTERFACE_POWER       0x08U
#define USB_DESCRIPTOR_TYPE_OTG                   0x09U
#define USB_DESCRIPTOR_TYPE_DEBUG                 0x0AU
#define USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION 0x0BU
#define USB_DESCRIPTOR_TYPE_BINARY_OBJECT_STORE   0x0FU
#define USB_DESCRIPTOR_TYPE_DEVICE_CAPABILITY     0x10U

#define USB_DESCRIPTOR_TYPE_FUNCTIONAL 0x21U

// Class Specific Descriptor
#define USB_CS_DESCRIPTOR_TYPE_DEVICE        0x21U
#define USB_CS_DESCRIPTOR_TYPE_CONFIGURATION 0x22U
#define USB_CS_DESCRIPTOR_TYPE_STRING        0x23U
#define USB_CS_DESCRIPTOR_TYPE_INTERFACE     0x24U
#define USB_CS_DESCRIPTOR_TYPE_ENDPOINT      0x25U

#define USB_DESCRIPTOR_TYPE_SUPERSPEED_ENDPOINT_COMPANION     0x30U
#define USB_DESCRIPTOR_TYPE_SUPERSPEED_ISO_ENDPOINT_COMPANION 0x31U

/* USB Device Classes */
#define USB_DEVICE_CLASS_RESERVED      0x00
#define USB_DEVICE_CLASS_AUDIO         0x01
#define USB_DEVICE_CLASS_CDC           0x02
#define USB_DEVICE_CLASS_HID           0x03
#define USB_DEVICE_CLASS_MONITOR       0x04
#define USB_DEVICE_CLASS_PHYSICAL      0x05
#define USB_DEVICE_CLASS_IMAGE         0x06
#define USB_DEVICE_CLASS_PRINTER       0x07
#define USB_DEVICE_CLASS_MASS_STORAGE  0x08
#define USB_DEVICE_CLASS_HUB           0x09
#define USB_DEVICE_CLASS_CDC_DATA      0x0a
#define USB_DEVICE_CLASS_SMART_CARD    0x0b
#define USB_DEVICE_CLASS_SECURITY      0x0d
#define USB_DEVICE_CLASS_VIDEO         0x0e
#define USB_DEVICE_CLASS_HEALTHCARE    0x0f
#define USB_DEVICE_CLASS_DIAG_DEVICE   0xdc
#define USB_DEVICE_CLASS_WIRELESS      0xe0
#define USB_DEVICE_CLASS_MISC          0xef
#define USB_DEVICE_CLASS_APP_SPECIFIC  0xfe
#define USB_DEVICE_CLASS_VEND_SPECIFIC 0xff

/* usb string index define */
#define USB_STRING_LANGID_INDEX    0x00
#define USB_STRING_MFC_INDEX       0x01
#define USB_STRING_PRODUCT_INDEX   0x02
#define USB_STRING_SERIAL_INDEX    0x03
#define USB_STRING_CONFIG_INDEX    0x04
#define USB_STRING_INTERFACE_INDEX 0x05
#define USB_STRING_OS_INDEX        0x06
#define USB_STRING_MAX             USB_STRING_OS_INDEX
/*
 * Devices supporting Microsoft OS Descriptors store special string
 * descriptor at fixed index (0xEE). It is read when a new device is
 * attached to a computer for the first time.
 */
#define USB_OSDESC_STRING_DESC_INDEX 0xEE

/* bmAttributes in Configuration Descriptor */
#define USB_CONFIG_POWERED_MASK  0x40
#define USB_CONFIG_BUS_POWERED   0x80
#define USB_CONFIG_SELF_POWERED  0xC0
#define USB_CONFIG_REMOTE_WAKEUP 0x20

/* bMaxPower in Configuration Descriptor */
#define USB_CONFIG_POWER_MA(mA) ((mA) / 2)

/* bEndpointAddress in Endpoint Descriptor */
#define USB_ENDPOINT_DIRECTION_MASK 0x80
#define USB_ENDPOINT_OUT(addr)      ((addr) | 0x00)
#define USB_ENDPOINT_IN(addr)       ((addr) | 0x80)

/* bmAttributes in Endpoint Descriptor */
#define USB_ENDPOINT_TYPE_MASK               0x03
#define USB_ENDPOINT_TYPE_CONTROL            0x00
#define USB_ENDPOINT_TYPE_ISOCHRONOUS        0x01
#define USB_ENDPOINT_TYPE_BULK               0x02
#define USB_ENDPOINT_TYPE_INTERRUPT          0x03
#define USB_ENDPOINT_SYNC_MASK               0x0C
#define USB_ENDPOINT_SYNC_NO_SYNCHRONIZATION 0x00
#define USB_ENDPOINT_SYNC_ASYNCHRONOUS       0x04
#define USB_ENDPOINT_SYNC_ADAPTIVE           0x08
#define USB_ENDPOINT_SYNC_SYNCHRONOUS        0x0C
#define USB_ENDPOINT_USAGE_MASK              0x30
#define USB_ENDPOINT_USAGE_DATA              0x00
#define USB_ENDPOINT_USAGE_FEEDBACK          0x10
#define USB_ENDPOINT_USAGE_IMPLICIT_FEEDBACK 0x20
#define USB_ENDPOINT_USAGE_RESERVED          0x30

/* bDevCapabilityType in Device Capability Descriptor */
#define USB_DEVICE_CAPABILITY_WIRELESS_USB                1
#define USB_DEVICE_CAPABILITY_USB_2_0_EXTENSION           2
#define USB_DEVICE_CAPABILITY_SUPERSPEED_USB              3
#define USB_DEVICE_CAPABILITY_CONTAINER_ID                4
#define USB_DEVICE_CAPABILITY_PLATFORM                    5
#define USB_DEVICE_CAPABILITY_POWER_DELIVERY_CAPABILITY   6
#define USB_DEVICE_CAPABILITY_BATTERY_INFO_CAPABILITY     7
#define USB_DEVICE_CAPABILITY_PD_CONSUMER_PORT_CAPABILITY 8
#define USB_DEVICE_CAPABILITY_PD_PROVIDER_PORT_CAPABILITY 9
#define USB_DEVICE_CAPABILITY_SUPERSPEED_PLUS             10
#define USB_DEVICE_CAPABILITY_PRECISION_TIME_MEASUREMENT  11
#define USB_DEVICE_CAPABILITY_WIRELESS_USB_EXT            12

#define USB_BOS_CAPABILITY_EXTENSION 0x02
#define USB_BOS_CAPABILITY_PLATFORM  0x05

/* Setup packet definition used to read raw data from USB line */
struct usb_setup_packet {
    __packed union {
        uint8_t bmRequestType; /* bmRequestType */
        struct
        {
            uint8_t Recipient : 5; /* D4..0: Recipient */
            uint8_t Type      : 2; /* D6..5: Type */
            uint8_t Dir       : 1; /* D7:    Data Phase Txsfer Direction */
        } bmRequestType_b;
    };
    uint8_t bRequest;
    __packed union {
        uint16_t wValue; /* wValue */
        struct
        {
            uint8_t wValueL;
            uint8_t wValueH;
        };
    };
    __packed union {
        uint16_t wIndex; /* wIndex */
        struct
        {
            uint8_t wIndexL;
            uint8_t wIndexH;
        };
    };
    uint16_t wLength;
} __packed;

/** Standard Device Descriptor */
struct usb_device_descriptor {
    uint8_t bLength;            /* Descriptor size in bytes = 18 */
    uint8_t bDescriptorType;    /* DEVICE descriptor type = 1 */
    uint16_t bcdUSB;            /* USB spec in BCD, e.g. 0x0200 */
    uint8_t bDeviceClass;       /* Class code, if 0 see interface */
    uint8_t bDeviceSubClass;    /* Sub-Class code, 0 if class = 0 */
    uint8_t bDeviceProtocol;    /* Protocol, if 0 see interface */
    uint8_t bMaxPacketSize0;    /* Endpoint 0 max. size */
    uint16_t idVendor;          /* Vendor ID per USB-IF */
    uint16_t idProduct;         /* Product ID per manufacturer */
    uint16_t bcdDevice;         /* Device release # in BCD */
    uint8_t iManufacturer;      /* Index to manufacturer string */
    uint8_t iProduct;           /* Index to product string */
    uint8_t iSerialNumber;      /* Index to serial number string */
    uint8_t bNumConfigurations; /* Number of possible configurations */
} __packed;

/** USB device_qualifier descriptor */
struct usb_device_qualifier_descriptor {
    uint8_t bLength;            /* Descriptor size in bytes = 10 */
    uint8_t bDescriptorType;    /* DEVICE QUALIFIER type = 6 */
    uint16_t bcdUSB;            /* USB spec in BCD, e.g. 0x0200 */
    uint8_t bDeviceClass;       /* Class code, if 0 see interface */
    uint8_t bDeviceSubClass;    /* Sub-Class code, 0 if class = 0 */
    uint8_t bDeviceProtocol;    /* Protocol, if 0 see interface */
    uint8_t bMaxPacketSize;     /* Endpoint 0 max. size */
    uint8_t bNumConfigurations; /* Number of possible configurations */
    uint8_t bReserved;          /* Reserved = 0 */
} __packed;

/** Standard Configuration Descriptor */
struct usb_configuration_descriptor {
    uint8_t bLength;             /* Descriptor size in bytes = 9 */
    uint8_t bDescriptorType;     /* CONFIGURATION type = 2 or 7 */
    uint16_t wTotalLength;       /* Length of concatenated descriptors */
    uint8_t bNumInterfaces;      /* Number of interfaces, this config. */
    uint8_t bConfigurationValue; /* Value to set this config. */
    uint8_t iConfiguration;      /* Index to configuration string */
    uint8_t bmAttributes;        /* Config. characteristics */
    uint8_t bMaxPower;           /* Max.power from bus, 2mA units */
} __packed;

/** Standard Interface Descriptor */
struct usb_interface_descriptor {
    uint8_t bLength;            /* Descriptor size in bytes = 9 */
    uint8_t bDescriptorType;    /* INTERFACE descriptor type = 4 */
    uint8_t bInterfaceNumber;   /* Interface no.*/
    uint8_t bAlternateSetting;  /* Value to select this IF */
    uint8_t bNumEndpoints;      /* Number of endpoints excluding 0 */
    uint8_t bInterfaceClass;    /* Class code, 0xFF = vendor */
    uint8_t bInterfaceSubClass; /* Sub-Class code, 0 if class = 0 */
    uint8_t bInterfaceProtocol; /* Protocol, 0xFF = vendor */
    uint8_t iInterface;         /* Index to interface string */
} __packed;

/** Standard Endpoint Descriptor */
struct usb_endpoint_descriptor {
    uint8_t bLength;          /* Descriptor size in bytes = 7 */
    uint8_t bDescriptorType;  /* ENDPOINT descriptor type = 5 */
    uint8_t bEndpointAddress; /* Endpoint # 0 - 15 | IN/OUT */
    uint8_t bmAttributes;     /* Transfer type */
    uint16_t wMaxPacketSize;  /* Bits 10:0 = max. packet size */
    uint8_t bInterval;        /* Polling interval in (micro) frames */
} __packed;

/** Unicode (UTF16LE) String Descriptor */
struct usb_string_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bString;
} __packed;

/* USB Interface Association Descriptor */
struct usb_interface_association_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bFirstInterface;
    uint8_t bInterfaceCount;
    uint8_t bFunctionClass;
    uint8_t bFunctionSubClass;
    uint8_t bFunctionProtocol;
    uint8_t iFunction;
} __packed;

/* MS OS 1.0 string descriptor */
struct usb_msosv1_string_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bString[14];
    uint8_t bMS_VendorCode; /* Vendor Code, used for a control request */
    uint8_t bPad;           /* Padding byte for VendorCode look as UTF16 */
} __packed;

/* MS OS 1.0 Header descriptor */
struct usb_msosv1_compat_id_header_descriptor {
    uint32_t dwLength;
    uint16_t bcdVersion;
    uint16_t wIndex;
    uint8_t bCount;
    uint8_t reserved[7];
} __packed;

/* MS OS 1.0 Function descriptor */
struct usb_msosv1_comp_id_function_descriptor {
    uint8_t bFirstInterfaceNumber;
    uint8_t reserved1;
    uint8_t compatibleID[8];
    uint8_t subCompatibleID[8];
    uint8_t reserved2[6];
} __packed;

#define usb_msosv1_comp_id_create(x)                                         \
    struct usb_msosv1_comp_id {                                              \
        struct usb_msosv1_compat_id_header_descriptor compat_id_header;      \
        struct usb_msosv1_comp_id_function_descriptor compat_id_function[x]; \
    };

struct usb_msosv1_descriptor {
    uint8_t *string;
    uint8_t string_len;
    uint8_t vendor_code;
    uint8_t *compat_id;
    uint16_t compat_id_len;
    uint8_t *comp_id_property;
    uint16_t comp_id_property_len;
};

/* MS OS 2.0 Header descriptor */
struct usb_msosv2_header_descriptor {
    uint32_t dwLength;
    uint16_t bcdVersion;
    uint16_t wIndex;
    uint8_t bCount;
} __packed;

/*Microsoft OS 2.0 set header descriptor*/
struct usb_msosv2_set_header_descriptor {
    uint16_t wLength;
    uint16_t wDescriptorType;
    uint32_t dwWindowsVersion;
    uint16_t wDescriptorSetTotalLength;
} __packed;

/* Microsoft OS 2.0 compatibleID descriptor*/
struct usb_msosv2_comp_id_descriptor {
    uint16_t wLength;
    uint16_t wDescriptorType;
    uint8_t compatibleID[8];
    uint8_t subCompatibleID[8];
} __packed;

/* MS OS 2.0 property descriptor */
struct usb_msosv2_property_descriptor {
    uint16_t wLength;
    uint16_t wDescriptorType;
    uint32_t dwPropertyDataType;
    uint16_t wPropertyNameLength;
    const char *bPropertyName;
    uint32_t dwPropertyDataLength;
    const char *bPropertyData;
};

/* Microsoft OS 2.0 subset function descriptor  */
struct usb_msosv2_subset_function_descriptor {
    uint16_t wLength;
    uint16_t wDescriptorType;
    uint8_t bFirstInterface;
    uint8_t bReserved;
    uint16_t wSubsetLength;
} __packed;

struct usb_msosv2_descriptor {
    uint8_t *compat_id;
    uint16_t compat_id_len;
    uint8_t vendor_code;
};

/* BOS header Descriptor */
struct usb_bos_header_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wTotalLength;
    uint8_t bNumDeviceCaps;
} __packed;

/* BOS Capability platform Descriptor */
struct usb_bos_capability_platform_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDevCapabilityType;
    uint8_t bReserved;
    uint8_t PlatformCapabilityUUID[16];
} __packed;

/* BOS Capability MS OS Descriptors version 2 */
struct usb_bos_capability_msosv2_descriptor {
    uint32_t dwWindowsVersion;
    uint16_t wMSOSDescriptorSetTotalLength;
    uint8_t bVendorCode;
    uint8_t bAltEnumCode;
} __packed;

/* BOS Capability webusb */
struct usb_bos_capability_webusb_descriptor {
    uint16_t bcdVersion;
    uint8_t bVendorCode;
    uint8_t iLandingPage;
} __packed;

/* BOS Capability extension Descriptor*/
struct usb_bos_capability_extension_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDevCapabilityType;
    uint32_t bmAttributes;
} __packed;

/* Microsoft OS 2.0 Platform Capability Descriptor
* See https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/
* microsoft-defined-usb-descriptors
* Adapted from the source:
* https://github.com/sowbug/weblight/blob/master/firmware/webusb.c
* (BSD-2) Thanks http://janaxelson.com/files/ms_os_20_descriptors.c
*/
struct usb_bos_capability_platform_msosv2_descriptor {
    struct usb_bos_capability_platform_descriptor platform_msos;
    struct usb_bos_capability_msosv2_descriptor data_msosv2;
} __packed;

/* WebUSB Platform Capability Descriptor:
* https://wicg.github.io/webusb/#webusb-platform-capability-descriptor
*/
struct usb_bos_capability_platform_webusb_descriptor {
    struct usb_bos_capability_platform_descriptor platform_webusb;
    struct usb_bos_capability_webusb_descriptor data_webusb;
} __packed;

struct usb_webusb_url_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bScheme;
    char URL[];
} __packed;

struct usb_bos_descriptor {
    uint8_t *string;
    uint32_t string_len;
};

/* USB Device Capability Descriptor */
struct usb_device_capability_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDevCapabilityType;
} __packed;

/** USB descriptor header */
struct usb_desc_header {
    uint8_t bLength;         /**< descriptor length */
    uint8_t bDescriptorType; /**< descriptor type */
};
// clang-format off
#define USB_DEVICE_DESCRIPTOR_INIT(bcdUSB, bDeviceClass, bDeviceSubClass, bDeviceProtocol, idVendor, idProduct, bcdDevice, bNumConfigurations) \
    0x12,                       /* bLength */                                                                                              \
    USB_DESCRIPTOR_TYPE_DEVICE, /* bDescriptorType */                                                                                      \
    WBVAL(bcdUSB),              /* bcdUSB */                                                                                               \
    bDeviceClass,               /* bDeviceClass */                                                                                         \
    bDeviceSubClass,            /* bDeviceSubClass */                                                                                      \
    bDeviceProtocol,            /* bDeviceProtocol */                                                                                      \
    0x40,                       /* bMaxPacketSize */                                                                                       \
    WBVAL(idVendor),            /* idVendor */                                                                                             \
    WBVAL(idProduct),           /* idProduct */                                                                                            \
    WBVAL(bcdDevice),           /* bcdDevice */                                                                                            \
    USB_STRING_MFC_INDEX,       /* iManufacturer */                                                                                        \
    USB_STRING_PRODUCT_INDEX,   /* iProduct */                                                                                             \
    USB_STRING_SERIAL_INDEX,    /* iSerial */                                                                                              \
    bNumConfigurations          /* bNumConfigurations */

#define USB_CONFIG_DESCRIPTOR_INIT(wTotalLength, bNumInterfaces, bConfigurationValue, bmAttributes, bMaxPower) \
    0x09,                              /* bLength */                                                       \
    USB_DESCRIPTOR_TYPE_CONFIGURATION, /* bDescriptorType */                                               \
    WBVAL(wTotalLength),               /* wTotalLength */                                                  \
    bNumInterfaces,                    /* bNumInterfaces */                                                \
    bConfigurationValue,               /* bConfigurationValue */                                           \
    0x00,                              /* iConfiguration */                                                \
    bmAttributes,                      /* bmAttributes */                                                  \
    USB_CONFIG_POWER_MA(bMaxPower)     /* bMaxPower */

#define USB_INTERFACE_DESCRIPTOR_INIT(bInterfaceNumber, bAlternateSetting, bNumEndpoints,                  \
                                      bInterfaceClass, bInterfaceSubClass, bInterfaceProtocol, iInterface) \
    0x09,                          /* bLength */                                                       \
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType */                                               \
    bInterfaceNumber,              /* bInterfaceNumber */                                              \
    bAlternateSetting,             /* bAlternateSetting */                                             \
    bNumEndpoints,                 /* bNumEndpoints */                                                 \
    bInterfaceClass,               /* bInterfaceClass */                                               \
    bInterfaceSubClass,            /* bInterfaceSubClass */                                            \
    bInterfaceProtocol,            /* bInterfaceProtocol */                                            \
    iInterface                     /* iInterface */

#define USB_ENDPOINT_DESCRIPTOR_INIT(bEndpointAddress, bmAttributes, wMaxPacketSize, bInterval) \
    0x07,                         /* bLength */                                             \
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType */                                     \
    bEndpointAddress,             /* bEndpointAddress */                                    \
    bmAttributes,                 /* bmAttributes */                                        \
    WBVAL(wMaxPacketSize),        /* wMaxPacketSize */                                      \
    bInterval                     /* bInterval */

#define USB_IAD_INIT(bFirstInterface, bInterfaceCount, bFunctionClass, bFunctionSubClass, bFunctionProtocol) \
    0x08,                                      /* bLength */                                             \
    USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION, /* bDescriptorType */                                     \
    bFirstInterface,                           /* bFirstInterface */                                     \
    bInterfaceCount,                           /* bInterfaceCount */                                     \
    bFunctionClass,                            /* bFunctionClass */                                      \
    bFunctionSubClass,                         /* bFunctionSubClass */                                   \
    bFunctionProtocol,                         /* bFunctionProtocol */                                   \
    0x00                                       /* iFunction */

#define USB_LANGID_INIT(id)                           \
    0x04,                           /* bLength */     \
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */ \
    WBVAL(id)                   /* wLangID0 */
// clang-format on
#endif