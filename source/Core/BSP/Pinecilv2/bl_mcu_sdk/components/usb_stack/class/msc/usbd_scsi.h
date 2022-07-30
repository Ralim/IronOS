/**
 * @file
 * @brief USB Mass Storage Class SCSI public header
 *
 * Header follows the Mass Storage Class Specification
 * (Mass_Storage_Specification_Overview_v1.4_2-19-2010.pdf) and
 * Mass Storage Class Bulk-Only Transport Specification
 * (usbmassbulk_10.pdf).
 * Header is limited to Bulk-Only Transfer protocol.
 */

#ifndef _USBD_SCSI_H_
#define _USBD_SCSI_H_

/* SCSI Commands */
#define SCSI_TEST_UNIT_READY             0x00
#define SCSI_REQUEST_SENSE               0x03
#define SCSI_FORMAT_UNIT                 0x04
#define SCSI_INQUIRY                     0x12
#define SCSI_MODE_SELECT6                0x15
#define SCSI_MODE_SENSE6                 0x1A
#define SCSI_START_STOP_UNIT             0x1B
#define SCSI_SEND_DIAGNOSTIC             0x1D
#define SCSI_PREVENT_ALLOW_MEDIA_REMOVAL 0x1E
#define SCSI_READ_FORMAT_CAPACITIES      0x23
#define SCSI_READ_CAPACITY10             0x25
#define SCSI_READ10                      0x28
#define SCSI_WRITE10                     0x2A
#define SCSI_VERIFY10                    0x2F
#define SCSI_SYNC_CACHE10                0x35
#define SCSI_READ12                      0xA8
#define SCSI_WRITE12                     0xAA
#define SCSI_MODE_SELECT10               0x55
#define SCSI_MODE_SENSE10                0x5A
#define SCSI_ATA_COMMAND_PASS_THROUGH16  0x85
#define SCSI_READ16                      0x88
#define SCSI_WRITE16                     0x8A
#define SCSI_VERIFY16                    0x8F
#define SCSI_SYNC_CACHE16                0x91
#define SCSI_SERVICE_ACTION_IN16         0x9E
#define SCSI_READ_CAPACITY16             0x9E
#define SCSI_SERVICE_ACTION_OUT16        0x9F
#define SCSI_ATA_COMMAND_PASS_THROUGH12  0xA1
#define SCSI_REPORT_ID_INFO              0xA3
#define SCSI_READ12                      0xA8
#define SCSI_SERVICE_ACTION_OUT12        0xA9
#define SCSI_SERVICE_ACTION_IN12         0xAB
#define SCSI_VERIFY12                    0xAF

/* SCSI Sense Key */
#define SCSI_SENSE_NONE            0x00
#define SCSI_SENSE_RECOVERED_ERROR 0x01
#define SCSI_SENSE_NOT_READY       0x02
#define SCSI_SENSE_MEDIUM_ERROR    0x03
#define SCSI_SENSE_HARDWARE_ERROR  0x04
#define SCSI_SENSE_ILLEGAL_REQUEST 0x05
#define SCSI_SENSE_UNIT_ATTENTION  0x06
#define SCSI_SENSE_DATA_PROTECT    0x07
#define SCSI_SENSE_FIRMWARE_ERROR  0x08
#define SCSI_SENSE_ABORTED_COMMAND 0x0b
#define SCSI_SENSE_EQUAL           0x0c
#define SCSI_SENSE_VOLUME_OVERFLOW 0x0d
#define SCSI_SENSE_MISCOMPARE      0x0e

/* Additional Sense Code */
#define SCSI_ASC_INVALID_CDB                     0x20U
#define SCSI_ASC_INVALID_FIELED_IN_COMMAND       0x24U
#define SCSI_ASC_PARAMETER_LIST_LENGTH_ERROR     0x1AU
#define SCSI_ASC_INVALID_FIELD_IN_PARAMETER_LIST 0x26U
#define SCSI_ASC_ADDRESS_OUT_OF_RANGE            0x21U
#define SCSI_ASC_MEDIUM_NOT_PRESENT              0x3AU
#define SCSI_ASC_MEDIUM_HAVE_CHANGED             0x28U
#define SCSI_ASC_WRITE_PROTECTED                 0x27U
#define SCSI_ASC_UNRECOVERED_READ_ERROR          0x11U
#define SCSI_ASC_WRITE_FAULT                     0x03U

#define READ_FORMAT_CAPACITY_DATA_LEN 0x0CU
#define READ_CAPACITY10_DATA_LEN      0x08U
#define REQUEST_SENSE_DATA_LEN        0x12U
#define STANDARD_INQUIRY_DATA_LEN     0x24U
#define MODE_SENSE6_DATA_LEN          0x17U
#define MODE_SENSE10_DATA_LEN         0x1BU

#define SCSI_MEDIUM_STATUS_UNLOCKED 0x00U
#define SCSI_MEDIUM_STATUS_LOCKED   0x01U
#define SCSI_MEDIUM_STATUS_EJECTED  0x02U

//--------------------------------------------------------------------+
// SCSI Primary Command (SPC-4)
//--------------------------------------------------------------------+

/// SCSI Test Unit Ready Command
typedef struct __packed {
    uint8_t cmd_code; ///< SCSI OpCode for \ref SCSI_CMD_TEST_UNIT_READY
    uint8_t lun;      ///< Logical Unit
    uint8_t reserved[3];
    uint8_t control;
} scsi_test_unit_ready_cmd_t;

/// SCSI Inquiry Command
typedef struct __packed {
    uint8_t cmd_code; ///< SCSI OpCode for \ref SCSI_CMD_INQUIRY
    uint8_t reserved1;
    uint8_t page_code;
    uint8_t reserved2;
    uint8_t alloc_length; ///< specifies the maximum number of bytes that USB host has allocated in the Data-In Buffer. An allocation length of zero specifies that no data shall be transferred.
    uint8_t control;
} scsi_inquiry_cmd_t, scsi_request_sense_cmd_t;

/// SCSI Inquiry Response Data
typedef struct __packed {
    uint8_t peripheral_device_type : 5;
    uint8_t peripheral_qualifier   : 3;

    uint8_t              : 7;
    uint8_t is_removable : 1;

    uint8_t version;

    uint8_t response_data_format : 4;
    uint8_t hierarchical_support : 1;
    uint8_t normal_aca           : 1;
    uint8_t                      : 2;

    uint8_t additional_length;

    uint8_t protect                    : 1;
    uint8_t                            : 2;
    uint8_t third_party_copy           : 1;
    uint8_t target_port_group_support  : 2;
    uint8_t access_control_coordinator : 1;
    uint8_t scc_support                : 1;

    uint8_t addr16            : 1;
    uint8_t                   : 3;
    uint8_t multi_port        : 1;
    uint8_t                   : 1; // vendor specific
    uint8_t enclosure_service : 1;
    uint8_t                   : 1;

    uint8_t         : 1; // vendor specific
    uint8_t cmd_que : 1;
    uint8_t         : 2;
    uint8_t sync    : 1;
    uint8_t wbus16  : 1;
    uint8_t         : 2;

    uint8_t vendor_id[8];   ///< 8 bytes of ASCII data identifying the vendor of the product.
    uint8_t product_id[16]; ///< 16 bytes of ASCII data defined by the vendor.
    uint8_t product_rev[4]; ///< 4 bytes of ASCII data defined by the vendor.
} scsi_inquiry_resp_t;

typedef struct __packed {
    uint8_t response_code : 7; ///< 70h - current errors, Fixed Format 71h - deferred errors, Fixed Format
    uint8_t valid         : 1;

    uint8_t reserved;

    uint8_t sense_key     : 4;
    uint8_t               : 1;
    uint8_t ili           : 1; ///< Incorrect length indicator
    uint8_t end_of_medium : 1;
    uint8_t filemark      : 1;

    uint32_t information;
    uint8_t add_sense_len;
    uint32_t command_specific_info;
    uint8_t add_sense_code;
    uint8_t add_sense_qualifier;
    uint8_t field_replaceable_unit_code;

    uint8_t sense_key_specific[3]; ///< sense key specific valid bit is bit 7 of key[0], aka MSB in Big Endian layout

} scsi_sense_fixed_resp_t;

typedef struct __packed {
    uint8_t cmd_code; ///< SCSI OpCode for \ref SCSI_CMD_MODE_SENSE_6

    uint8_t                          : 3;
    uint8_t disable_block_descriptor : 1;
    uint8_t                          : 4;

    uint8_t page_code    : 6;
    uint8_t page_control : 2;

    uint8_t subpage_code;
    uint8_t alloc_length;
    uint8_t control;
} scsi_mode_sense6_cmd_t;

// This is only a Mode parameter header(6).
typedef struct __packed {
    uint8_t data_len;
    uint8_t medium_type;

    uint8_t reserved     : 7;
    bool write_protected : 1;

    uint8_t block_descriptor_len;
} scsi_mode_sense6_resp_t;

typedef struct
{
    uint8_t cmd_code;

    uint8_t reserved1                : 3;
    uint8_t disable_block_descriptor : 1;
    uint8_t long_LBA                 : 1;
    uint8_t reserved2                : 3;

    uint8_t page_code    : 6;
    uint8_t page_control : 2;

    uint8_t subpage_code;

    uint8_t reserved3;
    uint8_t reserved4;
    uint8_t reserved5;

    uint8_t length[2];

    uint8_t control;
} scsi_mode_sense_10_cmd_t;

typedef struct
{
    uint8_t mode_data_length_high;
    uint8_t mode_data_length_low;
    uint8_t medium_type;

    uint8_t reserved1     : 4;
    uint8_t DPO_FUA       : 1; /**< [Disable Page Out] and [Force Unit Access] in the SCSI_READ10 command is valid or not */
    uint8_t reserved2     : 2;
    uint8_t write_protect : 1;

    uint8_t long_LBA  : 1;
    uint8_t reserved3 : 7;

    uint8_t reserved4;
    uint8_t block_desc_length[2];
} scsi_mode_10_resp_t;

typedef struct __packed {
    uint8_t cmd_code; ///< SCSI OpCode for \ref SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL
    uint8_t reserved[3];
    uint8_t prohibit_removal;
    uint8_t control;
} scsi_prevent_allow_medium_removal_t;

typedef struct __packed {
    uint8_t cmd_code;

    uint8_t immded : 1;
    uint8_t        : 7;

    uint8_t TU_RESERVED;

    uint8_t power_condition_mod : 4;
    uint8_t                     : 4;

    uint8_t start           : 1;
    uint8_t load_eject      : 1;
    uint8_t no_flush        : 1;
    uint8_t                 : 1;
    uint8_t power_condition : 4;

    uint8_t control;
} scsi_start_stop_unit_cmd_t;

//--------------------------------------------------------------------+
// SCSI MMC
//--------------------------------------------------------------------+
/// SCSI Read Format Capacity: Write Capacity
typedef struct __packed {
    uint8_t cmd_code;
    uint8_t reserved[6];
    uint16_t alloc_length;
    uint8_t control;
} scsi_read_format_capacity_cmd_t;

typedef struct __packed {
    uint8_t reserved[3];
    uint8_t list_length; /// must be 8*n, length in bytes of formattable capacity descriptor followed it.

    uint32_t block_num;      /// Number of Logical Blocks
    uint8_t descriptor_type; // 00: reserved, 01 unformatted media , 10 Formatted media, 11 No media present

    uint8_t reserved2;
    uint16_t block_size_u16;

} scsi_read_format_capacity_resp_t;

//--------------------------------------------------------------------+
// SCSI Block Command (SBC-3)
// NOTE: All data in SCSI command are in Big Endian
//--------------------------------------------------------------------+

/// SCSI Read Capacity 10 Command: Read Capacity
typedef struct __packed {
    uint8_t cmd_code; ///< SCSI OpCode for \ref SCSI_CMD_READ_CAPACITY_10
    uint8_t reserved1;
    uint32_t lba; ///< The first Logical Block Address (LBA) accessed by this command
    uint16_t reserved2;
    uint8_t partial_medium_indicator;
    uint8_t control;
} scsi_read_capacity10_cmd_t;

/// SCSI Read Capacity 10 Response Data
typedef struct
{
    uint32_t last_lba;   ///< The last Logical Block Address of the device
    uint32_t block_size; ///< Block size in bytes
} scsi_read_capacity10_resp_t;

/// SCSI Read 10 Command
typedef struct __packed {
    uint8_t cmd_code; ///< SCSI OpCode
    uint8_t reserved; // has LUN according to wiki
    uint32_t lba;     ///< The first Logical Block Address (LBA) accessed by this command
    uint8_t reserved2;
    uint16_t block_count; ///< Number of Blocks used by this command
    uint8_t control;
} scsi_read10_t, scsi_write10_t, scsi_read_write_10_t;

#endif /* ZEPHYR_INCLUDE_USB_CLASS_USB_CDC_H_ */
