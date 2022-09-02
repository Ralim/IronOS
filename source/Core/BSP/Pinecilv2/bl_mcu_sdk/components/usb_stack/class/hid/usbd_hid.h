/**
 * @file
 * @brief USB Human Interface Device (HID) Class public header
 *
 * Header follows Device Class Definition for Human Interface Devices (HID)
 * Version 1.11 document (HID1_11-1.pdf).
 */
#ifndef _USBD_HID_H_
#define _USBD_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* HID Protocol Codes */
#define HID_PROTOCOL_NONE     0x00
#define HID_PROTOCOL_BOOT     0x00
#define HID_PROTOCOL_KEYBOARD 0x01
#define HID_PROTOCOL_REPORT   0x01
#define HID_PROTOCOL_MOUSE    0x02

/* HID Class Descriptor Types */
#define HID_DESCRIPTOR_TYPE_HID          0x21
#define HID_DESCRIPTOR_TYPE_HID_REPORT   0x22
#define HID_DESCRIPTOR_TYPE_HID_PHYSICAL 0x23

/* HID Class Specific Requests */
#define HID_REQUEST_GET_REPORT   0x01
#define HID_REQUEST_GET_IDLE     0x02
#define HID_REQUEST_GET_PROTOCOL 0x03
#define HID_REQUEST_SET_REPORT   0x09
#define HID_REQUEST_SET_IDLE     0x0A
#define HID_REQUEST_SET_PROTOCOL 0x0B

/* HID Report Types */
#define HID_REPORT_INPUT   0x01
#define HID_REPORT_OUTPUT  0x02
#define HID_REPORT_FEATURE 0x03

/* HID Report Definitions */
struct usb_hid_class_subdescriptor {
    uint8_t bDescriptorType;
    uint16_t wDescriptorLength;
} __packed;

struct usb_hid_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdHID;
    uint8_t bCountryCode;
    uint8_t bNumDescriptors;

    /*
     * Specification says at least one Class Descriptor needs to
     * be present (Report Descriptor).
     */
    struct usb_hid_class_subdescriptor subdesc[1];
} __packed;

/* HID Items types */
#define ITEM_MAIN   0x0
#define ITEM_GLOBAL 0x1
#define ITEM_LOCAL  0x2

/* HID Main Items tags */
#define ITEM_TAG_INPUT          0x8
#define ITEM_TAG_OUTPUT         0x9
#define ITEM_TAG_COLLECTION     0xA
#define ITEM_TAG_COLLECTION_END 0xC

/* HID Global Items tags */
#define ITEM_TAG_USAGE_PAGE   0x0
#define ITEM_TAG_LOGICAL_MIN  0x1
#define ITEM_TAG_LOGICAL_MAX  0x2
#define ITEM_TAG_REPORT_SIZE  0x7
#define ITEM_TAG_REPORT_ID    0x8
#define ITEM_TAG_REPORT_COUNT 0x9

/* HID Local Items tags */
#define ITEM_TAG_USAGE     0x0
#define ITEM_TAG_USAGE_MIN 0x1
#define ITEM_TAG_USAGE_MAX 0x2

#define HID_ITEM(bTag, bType, bSize) (((bTag & 0xF) << 4) | \
                                      ((bType & 0x3) << 2) | (bSize & 0x3))

#define HID_MAIN_ITEM(bTag, bSize)   HID_ITEM(bTag, ITEM_MAIN, bSize)
#define HID_GLOBAL_ITEM(bTag, bSize) HID_ITEM(bTag, ITEM_GLOBAL, bSize)
#define HID_LOCAL_ITEM(bTag, bSize)  HID_ITEM(bTag, ITEM_LOCAL, bSize)

#define HID_MI_COLLECTION     HID_MAIN_ITEM(ITEM_TAG_COLLECTION, 1)
#define HID_MI_COLLECTION_END HID_MAIN_ITEM(ITEM_TAG_COLLECTION_END, \
                                            0)
#define HID_MI_INPUT  HID_MAIN_ITEM(ITEM_TAG_INPUT, 1)
#define HID_MI_OUTPUT HID_MAIN_ITEM(ITEM_TAG_OUTPUT, 1)

#define HID_GI_USAGE_PAGE        HID_GLOBAL_ITEM(ITEM_TAG_USAGE_PAGE, 1)
#define HID_GI_LOGICAL_MIN(size) HID_GLOBAL_ITEM(ITEM_TAG_LOGICAL_MIN, \
                                                 size)
#define HID_GI_LOGICAL_MAX(size) HID_GLOBAL_ITEM(ITEM_TAG_LOGICAL_MAX, \
                                                 size)
#define HID_GI_REPORT_SIZE HID_GLOBAL_ITEM(ITEM_TAG_REPORT_SIZE, \
                                           1)
#define HID_GI_REPORT_ID HID_GLOBAL_ITEM(ITEM_TAG_REPORT_ID, \
                                         1)
#define HID_GI_REPORT_COUNT HID_GLOBAL_ITEM(ITEM_TAG_REPORT_COUNT, \
                                            1)

#define HID_LI_USAGE           HID_LOCAL_ITEM(ITEM_TAG_USAGE, 1)
#define HID_LI_USAGE_MIN(size) HID_LOCAL_ITEM(ITEM_TAG_USAGE_MIN, \
                                              size)
#define HID_LI_USAGE_MAX(size) HID_LOCAL_ITEM(ITEM_TAG_USAGE_MAX, \
                                              size)

/* Defined in Universal Serial Bus HID Usage Tables version 1.11 */
#define USAGE_GEN_DESKTOP  0x01
#define USAGE_GEN_KEYBOARD 0x07
#define USAGE_GEN_LEDS     0x08
#define USAGE_GEN_BUTTON   0x09

/* Generic Desktop Page usages */
#define USAGE_GEN_DESKTOP_UNDEFINED 0x00
#define USAGE_GEN_DESKTOP_POINTER   0x01
#define USAGE_GEN_DESKTOP_MOUSE     0x02
#define USAGE_GEN_DESKTOP_JOYSTICK  0x04
#define USAGE_GEN_DESKTOP_GAMEPAD   0x05
#define USAGE_GEN_DESKTOP_KEYBOARD  0x06
#define USAGE_GEN_DESKTOP_KEYPAD    0x07
#define USAGE_GEN_DESKTOP_X         0x30
#define USAGE_GEN_DESKTOP_Y         0x31
#define USAGE_GEN_DESKTOP_WHEEL     0x38

/* Collection types */
#define COLLECTION_PHYSICAL    0x00
#define COLLECTION_APPLICATION 0x01

/* Example HID report descriptors */
/**
 * @brief Simple HID mouse report descriptor for n button mouse.
 *
 * @param bcnt  Button count. Allowed values from 1 to 8.
 */
#define HID_MOUSE_REPORT_DESC(bcnt)                                                                                          \
    {                                                                                                                        \
        /* USAGE_PAGE (Generic Desktop) */                                                                                   \
        HID_GI_USAGE_PAGE, USAGE_GEN_DESKTOP,                                             /* USAGE (Mouse) */                \
            HID_LI_USAGE, USAGE_GEN_DESKTOP_MOUSE,                                        /* COLLECTION (Application) */     \
            HID_MI_COLLECTION, COLLECTION_APPLICATION,                                    /* USAGE (Pointer) */              \
            HID_LI_USAGE, USAGE_GEN_DESKTOP_POINTER,                                      /* COLLECTION (Physical) */        \
            HID_MI_COLLECTION, COLLECTION_PHYSICAL, /* Bits used for button signalling */ /* USAGE_PAGE (Button) */          \
            HID_GI_USAGE_PAGE, USAGE_GEN_BUTTON,                                          /* USAGE_MINIMUM (Button 1) */     \
            HID_LI_USAGE_MIN(1), 0x01,                                                    /* USAGE_MAXIMUM (Button bcnt) */  \
            HID_LI_USAGE_MAX(1), bcnt,                                                    /* LOGICAL_MINIMUM (0) */          \
            HID_GI_LOGICAL_MIN(1), 0x00,                                                  /* LOGICAL_MAXIMUM (1) */          \
            HID_GI_LOGICAL_MAX(1), 0x01,                                                  /* REPORT_SIZE (1) */              \
            HID_GI_REPORT_SIZE, 0x01,                                                     /* REPORT_COUNT (bcnt) */          \
            HID_GI_REPORT_COUNT, bcnt,                                                    /* INPUT (Data,Var,Abs) */         \
            HID_MI_INPUT, 0x02, /* Unused bits */                                         /* REPORT_SIZE (8 - bcnt) */       \
            HID_GI_REPORT_SIZE, (8 - bcnt),                                               /* REPORT_COUNT (1) */             \
            HID_GI_REPORT_COUNT, 0x01,                                                    /* INPUT (Cnst,Ary,Abs) */         \
            HID_MI_INPUT, 0x01, /* X and Y axis, scroll */                                /* USAGE_PAGE (Generic Desktop) */ \
            HID_GI_USAGE_PAGE, USAGE_GEN_DESKTOP,                                         /* USAGE (X) */                    \
            HID_LI_USAGE, USAGE_GEN_DESKTOP_X,                                            /* USAGE (Y) */                    \
            HID_LI_USAGE, USAGE_GEN_DESKTOP_Y,                                            /* USAGE (WHEEL) */                \
            HID_LI_USAGE, USAGE_GEN_DESKTOP_WHEEL,                                        /* LOGICAL_MINIMUM (-127) */       \
            HID_GI_LOGICAL_MIN(1), -127,                                                  /* LOGICAL_MAXIMUM (127) */        \
            HID_GI_LOGICAL_MAX(1), 127,                                                   /* REPORT_SIZE (8) */              \
            HID_GI_REPORT_SIZE, 0x08,                                                     /* REPORT_COUNT (3) */             \
            HID_GI_REPORT_COUNT, 0x03,                                                    /* INPUT (Data,Var,Rel) */         \
            HID_MI_INPUT, 0x06,                                                           /* END_COLLECTION */               \
            HID_MI_COLLECTION_END,                                                        /* END_COLLECTION */               \
            HID_MI_COLLECTION_END,                                                                                           \
    }

/**
 * @brief Simple HID keyboard report descriptor.
 */
#define HID_KEYBOARD_REPORT_DESC()                                                                  \
    {                                                                                               \
        /* USAGE_PAGE (Generic Desktop) */                                                          \
        HID_GI_USAGE_PAGE, USAGE_GEN_DESKTOP,            /* USAGE (Keyboard) */                     \
            HID_LI_USAGE, USAGE_GEN_DESKTOP_KEYBOARD,    /* COLLECTION (Application) */             \
            HID_MI_COLLECTION, COLLECTION_APPLICATION,   /* USAGE_PAGE (Keypad) */                  \
            HID_GI_USAGE_PAGE, USAGE_GEN_DESKTOP_KEYPAD, /* USAGE_MINIMUM (Keyboard LeftControl) */ \
            HID_LI_USAGE_MIN(1), 0xE0,                   /* USAGE_MAXIMUM (Keyboard Right GUI) */   \
            HID_LI_USAGE_MAX(1), 0xE7,                   /* LOGICAL_MINIMUM (0) */                  \
            HID_GI_LOGICAL_MIN(1), 0x00,                 /* LOGICAL_MAXIMUM (1) */                  \
            HID_GI_LOGICAL_MAX(1), 0x01,                 /* REPORT_SIZE (1) */                      \
            HID_GI_REPORT_SIZE, 0x01,                    /* REPORT_COUNT (8) */                     \
            HID_GI_REPORT_COUNT, 0x08,                   /* INPUT (Data,Var,Abs) */                 \
            HID_MI_INPUT, 0x02,                          /* REPORT_SIZE (8) */                      \
            HID_GI_REPORT_SIZE, 0x08,                    /* REPORT_COUNT (1) */                     \
            HID_GI_REPORT_COUNT, 0x01,                   /* INPUT (Cnst,Var,Abs) */                 \
            HID_MI_INPUT, 0x03,                          /* REPORT_SIZE (1) */                      \
            HID_GI_REPORT_SIZE, 0x01,                    /* REPORT_COUNT (5) */                     \
            HID_GI_REPORT_COUNT, 0x05,                   /* USAGE_PAGE (LEDs) */                    \
            HID_GI_USAGE_PAGE, USAGE_GEN_LEDS,           /* USAGE_MINIMUM (Num Lock) */             \
            HID_LI_USAGE_MIN(1), 0x01,                   /* USAGE_MAXIMUM (Kana) */                 \
            HID_LI_USAGE_MAX(1), 0x05,                   /* OUTPUT (Data,Var,Abs) */                \
            HID_MI_OUTPUT, 0x02,                         /* REPORT_SIZE (3) */                      \
            HID_GI_REPORT_SIZE, 0x03,                    /* REPORT_COUNT (1) */                     \
            HID_GI_REPORT_COUNT, 0x01,                   /* OUTPUT (Cnst,Var,Abs) */                \
            HID_MI_OUTPUT, 0x03,                         /* REPORT_SIZE (8) */                      \
            HID_GI_REPORT_SIZE, 0x08,                    /* REPORT_COUNT (6) */                     \
            HID_GI_REPORT_COUNT, 0x06,                   /* LOGICAL_MINIMUM (0) */                  \
            HID_GI_LOGICAL_MIN(1), 0x00,                 /* LOGICAL_MAXIMUM (101) */                \
            HID_GI_LOGICAL_MAX(1), 0x65,                 /* USAGE_PAGE (Keypad) */                  \
            HID_GI_USAGE_PAGE, USAGE_GEN_DESKTOP_KEYPAD, /* USAGE_MINIMUM (Reserved) */             \
            HID_LI_USAGE_MIN(1), 0x00,                   /* USAGE_MAXIMUM (Keyboard Application) */ \
            HID_LI_USAGE_MAX(1), 0x65,                   /* INPUT (Data,Ary,Abs) */                 \
            HID_MI_INPUT, 0x00,                          /* END_COLLECTION */                       \
            HID_MI_COLLECTION_END,                                                                  \
    }

/**
 * @brief HID keyboard button codes.
 */
enum hid_kbd_code {
    HID_KEY_A = 4,
    HID_KEY_B = 5,
    HID_KEY_C = 6,
    HID_KEY_D = 7,
    HID_KEY_E = 8,
    HID_KEY_F = 9,
    HID_KEY_G = 10,
    HID_KEY_H = 11,
    HID_KEY_I = 12,
    HID_KEY_J = 13,
    HID_KEY_K = 14,
    HID_KEY_L = 15,
    HID_KEY_M = 16,
    HID_KEY_N = 17,
    HID_KEY_O = 18,
    HID_KEY_P = 19,
    HID_KEY_Q = 20,
    HID_KEY_R = 21,
    HID_KEY_S = 22,
    HID_KEY_T = 23,
    HID_KEY_U = 24,
    HID_KEY_V = 25,
    HID_KEY_W = 26,
    HID_KEY_X = 27,
    HID_KEY_Y = 28,
    HID_KEY_Z = 29,
    HID_KEY_1 = 30,
    HID_KEY_2 = 31,
    HID_KEY_3 = 32,
    HID_KEY_4 = 33,
    HID_KEY_5 = 34,
    HID_KEY_6 = 35,
    HID_KEY_7 = 36,
    HID_KEY_8 = 37,
    HID_KEY_9 = 38,
    HID_KEY_0 = 39,
    HID_KEY_ENTER = 40,
    HID_KEY_ESC = 41,
    HID_KEY_BACKSPACE = 42,
    HID_KEY_TAB = 43,
    HID_KEY_SPACE = 44,
    HID_KEY_MINUS = 45,
    HID_KEY_EQUAL = 46,
    HID_KEY_LEFTBRACE = 47,
    HID_KEY_RIGHTBRACE = 48,
    HID_KEY_BACKSLASH = 49,
    HID_KEY_HASH = 50, /* Non-US # and ~ */
    HID_KEY_SEMICOLON = 51,
    HID_KEY_APOSTROPHE = 52,
    HID_KEY_GRAVE = 53,
    HID_KEY_COMMA = 54,
    HID_KEY_DOT = 55,
    HID_KEY_SLASH = 56,
    HID_KEY_CAPSLOCK = 57,
    HID_KEY_F1 = 58,
    HID_KEY_F2 = 59,
    HID_KEY_F3 = 60,
    HID_KEY_F4 = 61,
    HID_KEY_F5 = 62,
    HID_KEY_F6 = 63,
    HID_KEY_F7 = 64,
    HID_KEY_F8 = 65,
    HID_KEY_F9 = 66,
    HID_KEY_F10 = 67,
    HID_KEY_F11 = 68,
    HID_KEY_F12 = 69,
    HID_KEY_SYSRQ = 70, /* PRINTSCREEN */
    HID_KEY_SCROLLLOCK = 71,
    HID_KEY_PAUSE = 72,
    HID_KEY_INSERT = 73,
    HID_KEY_HOME = 74,
    HID_KEY_PAGEUP = 75,
    HID_KEY_DELETE = 76,
    HID_KEY_END = 77,
    HID_KEY_PAGEDOWN = 78,
    HID_KEY_RIGHT = 79,
    HID_KEY_LEFT = 80,
    HID_KEY_DOWN = 81,
    HID_KEY_UP = 82,
    HID_KEY_NUMLOCK = 83,
    HID_KEY_KPSLASH = 84,    /* NUMPAD DIVIDE */
    HID_KEY_KPASTERISK = 85, /* NUMPAD MULTIPLY */
    HID_KEY_KPMINUS = 86,
    HID_KEY_KPPLUS = 87,
    HID_KEY_KPENTER = 88,
    HID_KEY_KP_1 = 89,
    HID_KEY_KP_2 = 90,
    HID_KEY_KP_3 = 91,
    HID_KEY_KP_4 = 92,
    HID_KEY_KP_5 = 93,
    HID_KEY_KP_6 = 94,
    HID_KEY_KP_7 = 95,
    HID_KEY_KP_8 = 96,
    HID_KEY_KP_9 = 97,
    HID_KEY_KP_0 = 98,
};

/**
 * @brief HID keyboard modifiers.
 */
enum hid_kbd_modifier {
    HID_KBD_MODIFIER_NONE = 0x00,
    HID_KBD_MODIFIER_LEFT_CTRL = 0x01,
    HID_KBD_MODIFIER_LEFT_SHIFT = 0x02,
    HID_KBD_MODIFIER_LEFT_ALT = 0x04,
    HID_KBD_MODIFIER_LEFT_UI = 0x08,
    HID_KBD_MODIFIER_RIGHT_CTRL = 0x10,
    HID_KBD_MODIFIER_RIGHT_SHIFT = 0x20,
    HID_KBD_MODIFIER_RIGHT_ALT = 0x40,
    HID_KBD_MODIFIER_RIGHT_UI = 0x80,
};

/**
 * @brief HID keyboard LEDs.
 */
enum hid_kbd_led {
    HID_KBD_LED_NUM_LOCK = 0x01,
    HID_KBD_LED_CAPS_LOCK = 0x02,
    HID_KBD_LED_SCROLL_LOCK = 0x04,
    HID_KBD_LED_COMPOSE = 0x08,
    HID_KBD_LED_KANA = 0x10,
};

void usbd_hid_descriptor_register(uint8_t intf_num, const uint8_t *desc);
void usbd_hid_report_descriptor_register(uint8_t intf_num, const uint8_t *desc, uint32_t desc_len);
void usbd_hid_add_interface(usbd_class_t *class, usbd_interface_t *intf);
void usbd_hid_reset_state(void);
void usbd_hid_send_report(uint8_t ep, uint8_t *data, uint8_t len);
// clang-format off
void usbd_hid_set_request_callback( uint8_t intf_num,
                                    uint8_t (*get_report_callback)(uint8_t report_id, uint8_t report_type),
                                    void (*set_report_callback)(uint8_t report_id, uint8_t report_type, uint8_t *report, uint8_t report_len),
                                    uint8_t (*get_idle_callback)(uint8_t report_id),
                                    void (*set_idle_callback)(uint8_t report_id, uint8_t duration),
                                    void (*set_protocol_callback)(uint8_t protocol),
                                    uint8_t (*get_protocol_callback)(void));
// clang-format on
#ifdef __cplusplus
}
#endif

#endif /* _USB_HID_H_ */
