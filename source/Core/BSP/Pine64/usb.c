#include "drv_usb_dev.h"
#include "drv_usb_hw.h"
#include "drv_usbd_int.h"
#include "drv_usb_core.h"
#include "usbd_core.h"
#include "usbd_enum.h"

#define USB_STRING_LEN(unicode_chars) (sizeof(usb_desc_header) + ((unicode_chars) << 1U))

#define USBD_VID 0x0001U
#define USBD_PID 0x0001U
#define USB_DFU_CLASS 0xFEU
#define USB_DFU_SUBCLASS_UPGRADE 0x01U
#define USB_DFU_PROTOCOL_DFU 0x01U


typedef struct _usb_desc_str {
	usb_desc_header header;               /*!< descriptor header, including type and size. */
	uint16_t unicode_string[64];          /*!< unicode string data */
} usb_desc_str;

/* USB dfu function descriptor structure */
typedef struct {
	usb_desc_header header;               /*!< descriptor header, including type and size */
	uint8_t bmAttributes;                 /*!< DFU attributes */
	uint16_t wDetachTimeOut;              /*!< time, in milliseconds, that the device will wait after receipt of the DFU_DETACH request. If */
		uint16_t wTransferSize;               /*!< maximum number of bytes that the device can accept per control-write transaction */
	uint16_t bcdDFUVersion;               /*!< numeric expression identifying the version of the DFU Specification release. */
} usb_desc_dfu_func;


/* USB configuration descriptor structure */
typedef struct {
	usb_desc_config           config;
	usb_desc_itf              dfu_itf;
	usb_desc_dfu_func         dfu_func;
} usb_dfu_desc_config_set;


static const usb_desc_dev dfu_runtime_dev = {
	.header = {
		.bLength = USB_DEV_DESC_LEN,
		.bDescriptorType = USB_DESCTYPE_DEV,
	},
	.bcdUSB = 0x00U,
	.bDeviceClass = 0x00U,
	.bDeviceSubClass = 0x00U,
	.bDeviceProtocol = 0x00U,
	.bMaxPacketSize0 = USB_FS_EP0_MAX_LEN,
	.idVendor = USBD_VID,
	.idProduct = USBD_PID,
	.bcdDevice = 0x0100U,
	.iManufacturer = STR_IDX_MFC,
	.iProduct = STR_IDX_PRODUCT,
	.iSerialNumber = STR_IDX_SERIAL,
	.bNumberConfigurations = USBD_CFG_MAX_NUM,
};

static const usb_desc_LANGID desc_LANGID_ENG = {
	.header = {
		.bLength = sizeof(usb_desc_LANGID),
		.bDescriptorType = USB_DESCTYPE_STR,
	},
	.wLANGID = ENG_LANGID,
};

/* USB manufacture string */
static const usb_desc_str manufacturer_string = {
	.header = {
		.bLength         = USB_STRING_LEN(2U),
		.bDescriptorType = USB_DESCTYPE_STR,
	},
	.unicode_string = {'M', 'e'},
};

/* USB product string */
static const usb_desc_str product_string = {
	.header = {
		.bLength         = USB_STRING_LEN(7U),
		.bDescriptorType = USB_DESCTYPE_STR,
	},
	.unicode_string = {'P', 'i', 'n', 'e', 'c', 'i', 'l'},
};

/* USBD serial string */
static usb_desc_str serial_string = {
	.header = {
		.bLength         = USB_STRING_LEN(2U),
		.bDescriptorType = USB_DESCTYPE_STR,
	}
};

/* USB configure string */
static const usb_desc_str config_string = {
	.header = {
		.bLength         = USB_STRING_LEN(6U),
		.bDescriptorType = USB_DESCTYPE_STR,
	},
	.unicode_string = {'C', 'O', 'N', 'F', 'I', 'G'},
};

static const usb_desc_str interface_string = {
	.header = {
		.bLength         = USB_STRING_LEN(3U),
		.bDescriptorType = USB_DESCTYPE_STR,
	},
	.unicode_string = {'I', 'T', 'F'},
};

static const usb_dfu_desc_config_set dfu_runtime_config = {
	.config = {
		.header = {
			.bLength = sizeof(usb_desc_config),
			.bDescriptorType = USB_DESCTYPE_CONFIG,
		},
		.wTotalLength = sizeof(usb_dfu_desc_config_set),
		.bNumInterfaces = 0x01U,
		.bConfigurationValue = 0x01U,
		.iConfiguration = STR_IDX_CONFIG,
		.bmAttributes = 0b01000000,
		.bMaxPower = 0x32U,
	},
	.dfu_itf = {
		.header = {
			.bLength = sizeof(usb_desc_itf),
			.bDescriptorType = USB_DESCTYPE_ITF,
		},
		.bInterfaceNumber = 0x00U,
		.bAlternateSetting = 0x00U,
		.bNumEndpoints = 0x00U,
		.bInterfaceClass = USB_DFU_CLASS,
		// FIXME do we really need the transfersize and interface
		// string in runtime mode??
		.bInterfaceSubClass = USB_DFU_SUBCLASS_UPGRADE,
		.bInterfaceProtocol = USB_DFU_PROTOCOL_DFU,
		.iInterface = STR_IDX_ITF,
	},
	.dfu_func = {
		.header = {
			.bLength = sizeof(usb_desc_dfu_func),
			.bDescriptorType = 0x21,
		},
		.bmAttributes = 0b00001011,
		.wDetachTimeOut = 0x00FFU,
		.wTransferSize = 2048,
		.bcdDFUVersion = 0x011AU,
	},
};

static void *const dfu_runtime_strings[] = {
	[STR_IDX_LANGID]  = (uint8_t *)&desc_LANGID_ENG,
	[STR_IDX_MFC]     = (uint8_t *)&manufacturer_string,
	[STR_IDX_PRODUCT] = (uint8_t *)&product_string,
	[STR_IDX_SERIAL]  = (uint8_t *)&serial_string,
	[STR_IDX_CONFIG]  = (uint8_t *)&config_string,
	[STR_IDX_ITF]     = (uint8_t *)&interface_string,
};

static const usb_desc dfu_runtime = {
	.dev_desc = (uint8_t *)&dfu_runtime_dev,
	.config_desc = (uint8_t *)&dfu_runtime_config,
	.strings = dfu_runtime_strings,
};

static usb_core_driver driver = {
	.dev.desc = (uint8_t *)&dfu_runtime,
};

static uint8_t dfu_runtime_init (usb_dev *udev, uint8_t config_index)
{
	return USBD_OK;
}
#define dfu_runtime_deinit dfu_runtime_init

static uint8_t dfu_runtime_req_handler (usb_dev *udev, usb_req *req)
{
    return USBD_OK;
}

static usb_class_core dfu_runtime_class = {
	.init     = dfu_runtime_init,
	.deinit   = dfu_runtime_deinit,
	.req_proc = dfu_runtime_req_handler,
};

void setup_usb(void) {
	usb_timer_init();
	usb_rcu_config();
	usbd_init(&driver, USB_CORE_ENUM_FS, &dfu_runtime_class);
	usb_intr_config();
}

void USBFS_IRQHandler (void) {
	usbd_isr (&driver);
}
