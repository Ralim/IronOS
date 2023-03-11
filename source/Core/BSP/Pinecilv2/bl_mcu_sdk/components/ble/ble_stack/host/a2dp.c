/** @file
 * @brief Advance Audio Distribution Profile.
 */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <string.h>
#include <errno.h>
#include <atomic.h>
#include <byteorder.h>
#include <util.h>
#include <printk.h>
#include <assert.h>

#include <bluetooth.h>
#include <l2cap.h>
#include <avdtp.h>
#include <a2dp.h>
#include <sdp.h>

#define BT_DBG_ENABLED  IS_ENABLED(CONFIG_BT_DEBUG_A2DP)
#define LOG_MODULE_NAME bt_a2dp
#include "log.h"

#include "hci_core.h"
#include "conn_internal.h"
#include "avdtp_internal.h"
#include "a2dp_internal.h"
#include "a2dp-codec.h"
#include "oi_codec_sbc.h"

#define A2DP_NO_SPACE (-1)

struct bt_a2dp {
    struct bt_avdtp session;
};

typedef struct {
    OI_CODEC_SBC_DECODER_CONTEXT decoder_context;
    uint32_t context_data[CODEC_DATA_WORDS(SBC_MAX_CHANNELS, SBC_CODEC_FAST_FILTER_BUFFERS)];
    int16_t decode_buf[15 * SBC_MAX_SAMPLES_PER_FRAME * SBC_MAX_CHANNELS];
} A2DP_SBC_DECODER;

static A2DP_SBC_DECODER sbc_decoder;

/* Connections */
static struct bt_a2dp connection[CONFIG_BT_MAX_CONN];
static struct bt_avdtp_stream stream[CONFIG_BT_MAX_CONN];

static struct bt_sdp_attribute a2dp_attrs[] = {
    BT_SDP_NEW_SERVICE,
    BT_SDP_LIST(
        BT_SDP_ATTR_SVCLASS_ID_LIST,
        BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 3),
        BT_SDP_DATA_ELEM_LIST(
            { BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
              BT_SDP_ARRAY_16(BT_SDP_AUDIO_SINK_SVCLASS) }, )),
    BT_SDP_LIST(
        BT_SDP_ATTR_PROTO_DESC_LIST,
        BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 16),
        BT_SDP_DATA_ELEM_LIST(
            { BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 6),
              BT_SDP_DATA_ELEM_LIST(
                  { BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                    BT_SDP_ARRAY_16(BT_SDP_PROTO_L2CAP) },
                  { BT_SDP_TYPE_SIZE(BT_SDP_UINT16),
                    BT_SDP_ARRAY_16(BT_L2CAP_PSM_AVDTP) }, ) },
            { BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 6),
              BT_SDP_DATA_ELEM_LIST(
                  { BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                    BT_SDP_ARRAY_16(BT_L2CAP_PSM_AVDTP) },
                  { BT_SDP_TYPE_SIZE(BT_SDP_UINT16),
                    BT_SDP_ARRAY_16(0x0102) }, ) }, )),
    BT_SDP_LIST(
        BT_SDP_ATTR_PROFILE_DESC_LIST,
        BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 8),
        BT_SDP_DATA_ELEM_LIST(
            { BT_SDP_TYPE_SIZE_VAR(BT_SDP_SEQ8, 6),
              BT_SDP_DATA_ELEM_LIST(
                  { BT_SDP_TYPE_SIZE(BT_SDP_UUID16),
                    BT_SDP_ARRAY_16(BT_SDP_ADVANCED_AUDIO_SVCLASS) },
                  { BT_SDP_TYPE_SIZE(BT_SDP_UINT16),
                    BT_SDP_ARRAY_16(0x0102) }, ) }, )),
    BT_SDP_SERVICE_NAME("A2DP sink"),
};

static struct bt_sdp_record a2dp_rec = BT_SDP_RECORD(a2dp_attrs);

struct bt_a2dp_endpoint endpoint_1;
struct bt_a2dp_endpoint endpoint_2;

struct bt_a2dp_codec_sbc_params sbc_info;

void bt_a2dp_set_sbc_codec_info()
{
    sbc_info.config[0] =
        //Sampling Frequency
        A2DP_SBC_SAMP_FREQ_48000 |
        A2DP_SBC_SAMP_FREQ_44100 |
        A2DP_SBC_SAMP_FREQ_32000 |
        A2DP_SBC_SAMP_FREQ_16000 |
        //Channel Mode
        A2DP_SBC_CH_MODE_JOINT |
        A2DP_SBC_CH_MODE_STREO |
        A2DP_SBC_CH_MODE_DUAL |
        A2DP_SBC_CH_MODE_MONO;
    sbc_info.config[1] =
        //Block Length
        A2DP_SBC_BLK_LEN_16 |
        A2DP_SBC_BLK_LEN_12 |
        A2DP_SBC_BLK_LEN_8 |
        A2DP_SBC_BLK_LEN_4 |
        //Subbands
        A2DP_SBC_SUBBAND_8 |
        A2DP_SBC_SUBBAND_4 |
        //Allocation Method
        A2DP_SBC_ALLOC_MTHD_SNR |
        A2DP_SBC_ALLOC_MTHD_LOUDNESS;
    sbc_info.min_bitpool = 2;
    sbc_info.max_bitpool = 53;
}

void a2d_reset(struct bt_a2dp *a2dp_conn)
{
    (void)memset(a2dp_conn, 0, sizeof(struct bt_a2dp));
}

void stream_reset(struct bt_avdtp_stream *stream_conn)
{
    (void)memset(stream_conn, 0, sizeof(struct bt_avdtp_stream));
}

struct bt_a2dp *get_new_connection(struct bt_conn *conn)
{
    int8_t i, free;

    free = A2DP_NO_SPACE;

    if (!conn) {
        BT_ERR("Invalid Input (err: %d)", -EINVAL);
        return NULL;
    }

    /* Find a space */
    for (i = 0; i < CONFIG_BT_MAX_CONN; i++) {
        if (connection[i].session.br_chan.chan.conn == conn) {
            BT_DBG("Conn already exists");
            if (!connection[i].session.streams->chan.chan.conn) {
                BT_DBG("Create AV stream");
                return &connection[i];
            } else {
                BT_DBG("A2DP signal stream and AV stream already exists");
                return NULL;
            }
        }

        if (!connection[i].session.br_chan.chan.conn &&
            free == A2DP_NO_SPACE) {
            BT_DBG("Create signal stream");
            free = i;
        }
    }

    if (free == A2DP_NO_SPACE) {
        BT_DBG("More connection cannot be supported");
        return NULL;
    }

    /* Clean the memory area before returning */
    a2d_reset(&connection[free]);
    stream_reset(&stream[free]);
    connection[free].session.streams = &stream[free];

    return &connection[free];
}

int a2dp_accept(struct bt_conn *conn, struct bt_avdtp **session)
{
    struct bt_a2dp *a2dp_conn;

    a2dp_conn = get_new_connection(conn);
    if (!a2dp_conn) {
        return -ENOMEM;
    }

    *session = &(a2dp_conn->session);
    BT_DBG("session: %p", &(a2dp_conn->session));

    return 0;
}

int a2dp_sbc_decode_init()
{
    OI_STATUS status = OI_CODEC_SBC_DecoderReset(&sbc_decoder.decoder_context,
                                                 sbc_decoder.context_data,
                                                 sizeof(sbc_decoder.context_data),
                                                 2,
                                                 2,
                                                 false,
                                                 false);
    if (!OI_SUCCESS(status)) {
        BT_ERR("decode init failed with error: %d\n", status);
        return status;
    }

    return 0;
}

#if PCM_PRINTF
extern int16_t cool_edit[];
extern uint32_t byte_index;
#endif
int a2dp_sbc_decode_process(uint8_t media_data[], uint16_t data_len)
{
    //remove media header, expose sbc frame
    const OI_BYTE *data = media_data + 12 + 1;
    OI_UINT32 data_size = data_len - 12 - 1;

    if (data_size <= 0) {
        BT_ERR("empty packet\n");
        return -1;
    }

    if (data[0] != 0x9c) {
        BT_ERR("sbc frame syncword error \n");
    }

    OI_INT16 *pcm = sbc_decoder.decode_buf;
    OI_UINT32 pcm_size = sizeof(sbc_decoder.decode_buf);

    OI_INT16 frame_count = OI_CODEC_SBC_FrameCount((OI_BYTE *)data, data_size);
    BT_DBG("frame_count: %d\n", frame_count);

    for (int i = 0; i < frame_count; i++) {
        OI_STATUS status = OI_CODEC_SBC_DecodeFrame(&sbc_decoder.decoder_context,
                                                    &data,
                                                    &data_size,
                                                    pcm,
                                                    &pcm_size);
        if (!OI_SUCCESS(status)) {
            BT_ERR("decoding failure with error: %d \n", status);
            return -1;
        }

#if PCM_PRINTF
        memcpy((OI_INT8 *)cool_edit + byte_index, pcm, pcm_size);
        byte_index += pcm_size;
#endif
    }

    return 0;
}

/* Callback for incoming requests */
static struct bt_avdtp_ind_cb cb_ind = {
    /*TODO*/
};

/* The above callback structures need to be packed and passed to AVDTP */
static struct bt_avdtp_event_cb avdtp_cb = {
    .ind = &cb_ind,
    .accept = a2dp_accept
};

int bt_a2dp_init(void)
{
    int err;

    /* Register event handlers with AVDTP */
    err = bt_avdtp_register(&avdtp_cb);
    if (err < 0) {
        BT_ERR("A2DP registration failed");
        return err;
    }

    /* Register SDP record */
    err = bt_sdp_register_service(&a2dp_rec);
    if (err < 0) {
        BT_ERR("A2DP regist sdp record failed");
        return err;
    }

    int reg_1 = bt_a2dp_register_endpoint(&endpoint_1, BT_A2DP_AUDIO, BT_A2DP_SINK);
    int reg_2 = bt_a2dp_register_endpoint(&endpoint_2, BT_A2DP_AUDIO, BT_A2DP_SINK);
    if (reg_1 || reg_2) {
        BT_ERR("A2DP registration endpoint 1 failed");
        return err;
    }

    bt_a2dp_set_sbc_codec_info();

    err = a2dp_sbc_decode_init();
    if (err < 0) {
        BT_ERR("sbc codec init failed");
        return err;
    }

    BT_DBG("A2DP Initialized successfully.");
    return 0;
}

struct bt_a2dp *bt_a2dp_connect(struct bt_conn *conn)
{
    struct bt_a2dp *a2dp_conn;
    int err;

    a2dp_conn = get_new_connection(conn);
    if (!a2dp_conn) {
        BT_ERR("Cannot allocate memory");
        return NULL;
    }

    err = bt_avdtp_connect(conn, &(a2dp_conn->session));
    if (err < 0) {
        /* If error occurs, undo the saving and return the error */
        a2d_reset(a2dp_conn);
        BT_DBG("AVDTP Connect failed");
        return NULL;
    }

    BT_DBG("Connect request sent");
    return a2dp_conn;
}

int bt_a2dp_register_endpoint(struct bt_a2dp_endpoint *endpoint,
                              uint8_t media_type, uint8_t role)
{
    int err;

    BT_ASSERT(endpoint);

    err = bt_avdtp_register_sep(media_type, role, &(endpoint->info));
    if (err < 0) {
        return err;
    }

    return 0;
}
