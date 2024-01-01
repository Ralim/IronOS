#ifndef __OAD_H__
#define __OAD_H__

#include "types.h"
#include "hci_host.h"
#include "work_q.h"

#define LOCAL_MANU_CODE 0x2c00
#define LOCAL_FILE_VER  00000001

#define OAD_OPCODE_SIZE 1
//00070000-0745-4650-8d93-df59be2fc10a
#define BT_UUID_OAD BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x00070000, 0x0745, 0x4650, 0x8d93, 0xdf59be2fc10a))
//00070001-0745-4650-8d93-df59be2fc10a
#define BT_UUID_OAD_DATA_IN BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x00070001, 0x0745, 0x4650, 0x8d93, 0xdf59be2fc10a))
//00070002-0745-4650-8d93-df59be2fc10a
#define BT_UUID_OAD_DATA_OUT BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x00070002, 0x0745, 0x4650, 0x8d93, 0xdf59be2fc10a))

enum {
    OAD_SUCC = 0x00,
    OAD_ABORT,
    OAD_INVALID_IMAG,
    OAD_REQ_MORE_DATA,
    OAD_MALORMED_CMD,
    OAD_UPGRD_CMPLT,
    OAD_CHECK_HASH256_FAIL,
};

enum {
    OAD_CMD_IMAG_IDENTITY = 0x00,
    OAD_CMD_IMAG_BLOCK_REQ,
    OAD_CMD_IMAG_BLOCK_RESP,
    OAD_CMD_IMAG_UPGRD_END,
    OAD_CMD_IMAG_INFO,
};

struct oad_file_info {
    u16_t manu_code;
    u32_t file_ver;
} __packed;

#if defined(CONFIG_BT_SETTINGS)
struct oad_ef_info {
    struct oad_file_info file_info;
    u32_t file_offset;
    u32_t last_wflash_addr;
    u32_t upgrd_crc32;
} __packed;

#endif

struct oad_env_tag {
    struct oad_file_info file_info;
    u32_t cur_file_size;
    u32_t upgrd_file_ver;
    u32_t upgrd_file_size;
    u32_t upgrd_offset;
    u32_t upgrd_crc32;

    struct k_delayed_work upgrd_work;
    u32_t new_img_addr;
    u32_t w_img_end_addr;

    u32_t hosal_offset;
};

struct oad_image_identity_t {
    struct oad_file_info file_info;
    u32_t file_size;
    u32_t crc32;
} __packed;

struct oad_block_req_t {
    struct oad_file_info file_info;
    u32_t file_offset;
} __packed;

#define OAD_BLK_RSP_DATA_OFFSET 12
struct oad_block_rsp_t {
    uint8_t status;
    struct oad_file_info file_info;
    u32_t file_offset;
    u8_t data_size;
    u8_t *pdata;
} __packed;

struct oad_upgrd_end_t {
    u8_t status;
    struct oad_file_info file_info;
} __packed;

#endif //__OAD_H__
