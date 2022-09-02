/**
 * @file
 * @brief USB Audio Device Class public header
 *
 * Header follows below documentation:
 * - USB Device Class Definition for Audio Devices (audio10.pdf)
 *
 * Additional documentation considered a part of USB Audio v1.0:
 * - USB Device Class Definition for Audio Data Formats (frmts10.pdf)
 * - USB Device Class Definition for Terminal Types (termt10.pdf)
 */

#ifndef _USBD_AUDIO_H_
#define _USBD_AUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/** Audio Interface Subclass Codes
 * Refer to Table A-2 from audio10.pdf
 */
#define AUDIO_SUBCLASS_UNDEFINED      0x00
#define AUDIO_SUBCLASS_AUDIOCONTROL   0x01
#define AUDIO_SUBCLASS_AUDIOSTREAMING 0x02
#define AUDIO_SUBCLASS_MIDISTREAMING  0x03

#define AUDIO_PROTOCOL_UNDEFINED 0x00U

#define AUDIO_ENDPOINT_GENERAL 0x01U

/** Audio Class-Specific Control Interface Descriptor Subtypes
 * Refer to Table A-5 from audio10.pdf
 */
#define AUDIO_CONTROL_UNDEFINED       0x01U
#define AUDIO_CONTROL_HEADER          0x01U
#define AUDIO_CONTROL_INPUT_TERMINAL  0x02U
#define AUDIO_CONTROL_OUTPUT_TERMINAL 0x03U
#define AUDIO_CONTROL_MIXER_UNIT      0x04U
#define AUDIO_CONTROL_SELECTOR_UNIT   0x05U
#define AUDIO_CONTROL_FEATURE_UNIT    0x06U
#define AUDIO_CONTROL_PROCESSING_UNIT 0x07U
#define AUDIO_CONTROL_EXTENSION_UNIT  0x08U

/** Audio Class-Specific AS Interface Descriptor Subtypes
 * Refer to Table A-6 from audio10.pdf
 */
#define AUDIO_STREAMING_UNDEFINED       0x00U
#define AUDIO_STREAMING_GENERAL         0x01U
#define AUDIO_STREAMING_FORMAT_TYPE     0x02U
#define AUDIO_STREAMING_FORMAT_SPECIFIC 0x03U

/** Audio Class-Specific Request Codes
 * Refer to Table A-9 from audio10.pdf
 */
#define AUDIO_REQUEST_UNDEFINED 0x00
#define AUDIO_REQUEST_SET_CUR   0x01
#define AUDIO_REQUEST_GET_CUR   0x81
#define AUDIO_REQUEST_SET_MIN   0x02
#define AUDIO_REQUEST_GET_MIN   0x82
#define AUDIO_REQUEST_SET_MAX   0x03
#define AUDIO_REQUEST_GET_MAX   0x83
#define AUDIO_REQUEST_SET_RES   0x04
#define AUDIO_REQUEST_GET_RES   0x84
#define AUDIO_REQUEST_SET_MEM   0x05
#define AUDIO_REQUEST_GET_MEM   0x85
#define AUDIO_REQUEST_GET_STAT  0xFF

/* Feature Unit Control Bits */
#define AUDIO_CONTROL_MUTE              0x0001
#define AUDIO_CONTROL_VOLUME            0x0002
#define AUDIO_CONTROL_BASS              0x0004
#define AUDIO_CONTROL_MID               0x0008
#define AUDIO_CONTROL_TREBLE            0x0010
#define AUDIO_CONTROL_GRAPHIC_EQUALIZER 0x0020
#define AUDIO_CONTROL_AUTOMATIC_GAIN    0x0040
#define AUDIO_CONTROL_DEALY             0x0080
#define AUDIO_CONTROL_BASS_BOOST        0x0100
#define AUDIO_CONTROL_LOUDNESS          0x0200

/** Feature Unit Control Selectors
 * Refer to Table A-11 from audio10.pdf
 */
#define AUDIO_FU_CONTROL_MUTE              0x01
#define AUDIO_FU_CONTROL_VOLUME            0x02
#define AUDIO_FU_CONTROL_BASS              0x03
#define AUDIO_FU_CONTROL_MID               0x04
#define AUDIO_FU_CONTROL_TREBLE            0x05
#define AUDIO_FU_CONTROL_GRAPHIC_EQUALIZER 0x06
#define AUDIO_FU_CONTROL_AUTOMATIC_GAIN    0x07
#define AUDIO_FU_CONTROL_DELAY             0x08
#define AUDIO_FU_CONTROL_BASS_BOOST        0x09
#define AUDIO_FU_CONTROL_LOUDNESS          0x0A

/* Audio Descriptor Types */
#define AUDIO_UNDEFINED_DESCRIPTOR_TYPE     0x20
#define AUDIO_DEVICE_DESCRIPTOR_TYPE        0x21
#define AUDIO_CONFIGURATION_DESCRIPTOR_TYPE 0x22
#define AUDIO_STRING_DESCRIPTOR_TYPE        0x23
#define AUDIO_INTERFACE_DESCRIPTOR_TYPE     0x24
#define AUDIO_ENDPOINT_DESCRIPTOR_TYPE      0x25

/* Audio Data Format Type I Codes */
#define AUDIO_FORMAT_TYPE_I_UNDEFINED 0x0000
#define AUDIO_FORMAT_PCM              0x0001
#define AUDIO_FORMAT_PCM8             0x0002
#define AUDIO_FORMAT_IEEE_FLOAT       0x0003
#define AUDIO_FORMAT_ALAW             0x0004
#define AUDIO_FORMAT_MULAW            0x0005

/* Predefined Audio Channel Configuration Bits */
#define AUDIO_CHANNEL_M   0x0000 /* Mono */
#define AUDIO_CHANNEL_L   0x0001 /* Left Front */
#define AUDIO_CHANNEL_R   0x0002 /* Right Front */
#define AUDIO_CHANNEL_C   0x0004 /* Center Front */
#define AUDIO_CHANNEL_LFE 0x0008 /* Low Freq. Enhance. */
#define AUDIO_CHANNEL_LS  0x0010 /* Left Surround */
#define AUDIO_CHANNEL_RS  0x0020 /* Right Surround */
#define AUDIO_CHANNEL_LC  0x0040 /* Left of Center */
#define AUDIO_CHANNEL_RC  0x0080 /* Right of Center */
#define AUDIO_CHANNEL_S   0x0100 /* Surround */
#define AUDIO_CHANNEL_SL  0x0200 /* Side Left */
#define AUDIO_CHANNEL_SR  0x0400 /* Side Right */
#define AUDIO_CHANNEL_T   0x0800 /* Top */

#define AUDIO_FORMAT_TYPE_I   0x01
#define AUDIO_FORMAT_TYPE_II  0x02
#define AUDIO_FORMAT_TYPE_III 0x03

/** USB Terminal Types
 * Refer to Table 2-1 - Table 2-4 from termt10.pdf
 */
enum usb_audio_terminal_types {
    /* USB Terminal Types */
    USB_AUDIO_USB_UNDEFINED = 0x0100,
    USB_AUDIO_USB_STREAMING = 0x0101,
    USB_AUDIO_USB_VENDOR_SPEC = 0x01FF,

    /* Input Terminal Types */
    USB_AUDIO_IN_UNDEFINED = 0x0200,
    USB_AUDIO_IN_MICROPHONE = 0x0201,
    USB_AUDIO_IN_DESKTOP_MIC = 0x0202,
    USB_AUDIO_IN_PERSONAL_MIC = 0x0203,
    USB_AUDIO_IN_OM_DIR_MIC = 0x0204,
    USB_AUDIO_IN_MIC_ARRAY = 0x0205,
    USB_AUDIO_IN_PROC_MIC_ARRAY = 0x0205,

    /* Output Terminal Types */
    USB_AUDIO_OUT_UNDEFINED = 0x0300,
    USB_AUDIO_OUT_SPEAKER = 0x0301,
    USB_AUDIO_OUT_HEADPHONES = 0x0302,
    USB_AUDIO_OUT_HEAD_AUDIO = 0x0303,
    USB_AUDIO_OUT_DESKTOP_SPEAKER = 0x0304,
    USB_AUDIO_OUT_ROOM_SPEAKER = 0x0305,
    USB_AUDIO_OUT_COMM_SPEAKER = 0x0306,
    USB_AUDIO_OUT_LOW_FREQ_SPEAKER = 0x0307,

    /* Bi-directional Terminal Types */
    USB_AUDIO_IO_UNDEFINED = 0x0400,
    USB_AUDIO_IO_HANDSET = 0x0401,
    USB_AUDIO_IO_HEADSET = 0x0402,
    USB_AUDIO_IO_SPEAKERPHONE_ECHO_NONE = 0x0403,
    USB_AUDIO_IO_SPEAKERPHONE_ECHO_SUP = 0x0404,
    USB_AUDIO_IO_SPEAKERPHONE_ECHO_CAN = 0x0405,
};

/**
 * @warning Size of baInterface is 2 just to make it useable
 * for all kind of devices: headphones, microphone and headset.
 * Actual size of the struct should be checked by reading
 * .bLength.
 */
struct cs_ac_if_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint16_t bcdADC;
    uint16_t wTotalLength;
    uint8_t bInCollection;
    uint8_t baInterfaceNr[2];
} __packed;

struct input_terminal_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bTerminalID;
    uint16_t wTerminalType;
    uint8_t bAssocTerminal;
    uint8_t bNrChannels;
    uint16_t wChannelConfig;
    uint8_t iChannelNames;
    uint8_t iTerminal;
} __packed;

/**
 * @note Size of Feature unit descriptor is not fixed.
 * This structure is just a helper not a common type.
 */
struct feature_unit_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bUnitID;
    uint8_t bSourceID;
    uint8_t bControlSize;
    uint16_t bmaControls[1];
} __packed;

struct output_terminal_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bTerminalID;
    uint16_t wTerminalType;
    uint8_t bAssocTerminal;
    uint8_t bSourceID;
    uint8_t iTerminal;
} __packed;

struct as_cs_interface_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bTerminalLink;
    uint8_t bDelay;
    uint16_t wFormatTag;
} __packed;

struct format_type_i_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bFormatType;
    uint8_t bNrChannels;
    uint8_t bSubframeSize;
    uint8_t bBitResolution;
    uint8_t bSamFreqType;
    uint8_t tSamFreq[3];
} __packed;

struct std_as_ad_endpoint_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
    uint8_t bRefresh;
    uint8_t bSynchAddress;
} __packed;

struct cs_as_ad_ep_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bmAttributes;
    uint8_t bLockDelayUnits;
    uint16_t wLockDelay;
} __packed;

struct usbd_audio_control_info {
    uint16_t vol_min;
    uint16_t vol_max;
    uint16_t vol_res;
    uint16_t vol_current;
    uint8_t mute;
};

void usbd_audio_add_interface(usbd_class_t *class, usbd_interface_t *intf);
void usbd_audio_set_interface_callback(uint8_t value);
void usbd_audio_set_volume(uint8_t vol);
#ifdef __cplusplus
}
#endif

#endif /* _USB_AUDIO_H_ */
