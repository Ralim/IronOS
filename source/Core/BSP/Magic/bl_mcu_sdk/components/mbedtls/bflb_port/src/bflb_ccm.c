#include "common.h"
#include "mbedtls/aes.h"
#include "mbedtls/ccm.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "mbedtls/platform.h"
#include "mbedtls_port_config.h"
#include "mbedtls/cipher.h"

#define TEST_DATA_TOATAL_LEN (4096)

static unsigned char b_input[TEST_DATA_TOATAL_LEN] = { 0 };
static unsigned char b_output[TEST_DATA_TOATAL_LEN] = { 0 };
static unsigned int cbc_adata_round = 0;
static unsigned int cbc_pt_round = 0;
static unsigned int ctr_pt_round = 0;
static unsigned int ctr_ct_round = 0;

mbedtls_aes_context aes_ctx = { 0 };
void mbedtls_ccm_init(mbedtls_ccm_context *ctx)
{
    CHECK_PARAM(ctx != NULL);
    Sec_Eng_AES_Enable_BE(SEC_ENG_AES_ID0);
    arch_memset(ctx, 0, sizeof(mbedtls_ccm_context));
}

void mbedtls_ccm_free(mbedtls_ccm_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    arch_memset(ctx, 0, sizeof(mbedtls_ccm_context));
}

int mbedtls_ccm_setkey(mbedtls_ccm_context *ctx, mbedtls_cipher_id_t cipher, const unsigned char *key, unsigned int key_bitlen)
{
    ctx->cipher_ctx.cipher_ctx = (void *)&aes_ctx;

    if (cipher == MBEDTLS_CIPHER_ID_AES /*&&
        (ctx->cipher_ctx.cipher_info->mode == MBEDTLS_MODE_ECB ||
        ctx->cipher_ctx.cipher_info->mode == MBEDTLS_MODE_CBC ||
        ctx->cipher_ctx.cipher_info->mode == MBEDTLS_MODE_CFB ||
        ctx->cipher_ctx.cipher_info->mode == MBEDTLS_MODE_OFB ||
        ctx->cipher_ctx.cipher_info->mode == MBEDTLS_MODE_CTR ||
        ctx->cipher_ctx.cipher_info->mode == MBEDTLS_MODE_XTS)*/
    ) {
        ctx->cipher_ctx.operation = MBEDTLS_ENCRYPT;
        mbedtls_aes_setkey_enc(ctx->cipher_ctx.cipher_ctx, key, key_bitlen);
    } else {
        return (-1);
    }

    return (0);
}

static int get_cbc_mac_input(mbedtls_ccm_context *ctx, size_t length,
                             const unsigned char *iv, size_t iv_len,
                             const unsigned char *add, size_t add_len,
                             const unsigned char *input, size_t tag_len)
{
    int ret = -1;
    unsigned char i;
    unsigned char q;
    size_t len_left;
    unsigned char b[16];
    const unsigned char *src;
    cbc_adata_round = 0;
    cbc_pt_round = 0;

    if (tag_len == 2 || tag_len > 16 || tag_len % 2 != 0) {
        return (ret);
    }

    /* Also implies q is within bounds */
    if (iv_len < 7 || iv_len > 13) {
        return (ret);
    }

    if (add_len >= 0xFF00) {
        return (ret);
    }

    arch_memset(b_input, 0, sizeof(b_input));

    q = 16 - 1 - (unsigned char)iv_len;

    /*
     * rfc3610_Vector22
     *
     *length = 19
     *iv_len = 13
     *add_len = 12
     *
     * First block B_0:
     * 0              Flags   -> 61 (0_1_100_001)
     *1 ... 15-L     Nonce N -> 00 5B 8C CB  CD 9A F8 3C  96 96 76 6C  FA
     *16-L ... 15    l(m)    -> 19 (00 13)
     *
     * With flags as (bits):
     * 7        Reserved        0
     * 6        Adata           1
     * 5 .. 3   M'(t - 2) / 2   4
     * 2 .. 0   L'q - 1         1
     */
    /*

    */

    b[0] = 0;
    b[0] |= (add_len > 0) << 6;
    b[0] |= ((tag_len - 2) / 2) << 3;
    b[0] |= q - 1;

    arch_memcpy(b + 1, iv, iv_len);

    for (i = 0, len_left = length; i < q; i++, len_left >>= 8) {
        b[15 - i] = (unsigned char)(len_left & 0xFF);
    }

    if (len_left > 0) {
        return (ret);
    }

    arch_memcpy(b_input, b, 16);

    /*
     * If there is additional data, update CBC-MAC with
     * add_len, add, 0 (padding to a block boundary)
     */
    if (add_len > 0) {
        size_t use_len;
        len_left = add_len;
        src = add;

        arch_memset(b, 0, 16);

        /* if add_len more than 2^16-2^8 */

        /*
        if( (add_len >= ((1 << 16) - (1 << 8))) && (add_len < (1 << 32) ) )
        {
            b[0] = 0xFF;
            b[1] = 0xFE;
        }
        else if( (add_len >=((1 << 32) ) && (add_len < (1 << 64) ) )
        {
            b[0] = 0xFF;
            b[1] = 0xFF;
        }
        else if( (add_len > 0 ) && (add_len < ((1 << 16) - (1 << 8)) ) )
        {
            b[0] = (unsigned char)( ( add_len >> 8 ) & 0xFF );
            b[1] = (unsigned char)( ( add_len      ) & 0xFF );
        }
        */

        /* first round for adata */
        cbc_adata_round = 1;

        b[0] = (unsigned char)((add_len >> 8) & 0xFF);
        b[1] = (unsigned char)((add_len)&0xFF);

        use_len = len_left < 16 - 2 ? len_left : 16 - 2;
        arch_memcpy(b + 2, src, use_len);
        len_left -= use_len;
        src += use_len;

        arch_memcpy(b_input + cbc_adata_round * 16, b, 16);

        while (len_left > 0) {
            cbc_adata_round++;

            use_len = len_left > 16 ? 16 : len_left;

            arch_memcpy(b_input + cbc_adata_round * 16, src, use_len);

            len_left -= use_len;
            src += use_len;
        }
    }

    /*
     * Authenticate and {en,de}crypt the message.
     *
     * The only difference between encryption and decryption is
     * the respective order of authentication and {en,de}cryption.
     */
    len_left = length;
    src = input;

    /* Plaintext block is  followed by adata block*/
    cbc_pt_round = cbc_adata_round + 1;

    while (len_left > 0) {
        size_t use_len = len_left > 16 ? 16 : len_left;

        arch_memcpy(b_input + cbc_pt_round * 16, src, use_len);

        src += use_len;
        len_left -= use_len;

        cbc_pt_round++;
    }

    return (0);
}

static int get_ctr_enc_input(size_t length, const unsigned char *input)
{
    size_t len_left = 0;
    const unsigned char *src;

    arch_memset(b_input, 0, sizeof(b_input));

    len_left = length;
    src = input;

    // Copy CRC plaintext last block output to here as CTR first block input
    arch_memcpy(b_input, b_output + (cbc_pt_round - 1) * 16, 16);

    ctr_pt_round = 1;

    while (len_left > 0) {
        size_t use_len = len_left > 16 ? 16 : len_left;

        arch_memcpy(b_input + ctr_pt_round * 16, src, use_len);

        src += use_len;
        len_left -= use_len;

        ctr_pt_round++;
    }

    return (0);
}

static int get_ctr_dec_input(size_t length, const unsigned char *input, const unsigned char *tag, size_t tag_len)
{
    size_t len_left = 0;
    const unsigned char *src;
    ctr_ct_round = 0;

    arch_memset(b_input, 0, sizeof(b_input));

    len_left = length;
    src = input;

    // Copy tag in first block to calucate T
    arch_memcpy(b_input, tag, 16);

    ctr_ct_round = 1;

    while (len_left > 0) {
        size_t use_len = len_left > 16 ? 16 : len_left;

        arch_memcpy(b_input + ctr_ct_round * 16, src, use_len);

        src += use_len;
        len_left -= use_len;

        ctr_ct_round++;
    }

    return (0);
}

static int get_ctr_iv(unsigned char *ctr_iv, const unsigned char *iv, size_t iv_len)
{
    unsigned char q;

    if (iv_len < 7 || iv_len > 13) {
        return (-1);
    }

    q = 16 - 1 - (unsigned char)iv_len;
    ctr_iv[0] |= q - 1;
    arch_memcpy(ctr_iv + 1, iv, iv_len);

    return (0);
}

int mbedtls_ccm_encrypt_and_tag(mbedtls_ccm_context *ctx, size_t length, const unsigned char *iv, size_t iv_len, const unsigned char *add, size_t add_len,
                                const unsigned char *input, unsigned char *output, unsigned char *tag, size_t tag_len)
{
    // Use cbc, clear iv to zero for CBC-MAC
    unsigned char cbc_iv[16] = { 0 };
    unsigned char ctr_iv[16] = { 0 };
    size_t cbc_length = 0;
    size_t ctr_length = 0;
    size_t nc_off = 0;
    unsigned char stream_block[16] = { 0 };
    int ret = 0;

    ret = get_cbc_mac_input(ctx, length, iv, iv_len, add, add_len, input, tag_len);

    if (ret) {
        return (ret);
    }

    cbc_length = 16 * cbc_pt_round;

    arch_memset(b_output, 0, sizeof(b_output));

    aes_ctx.nr = 0;
    mbedtls_aes_crypt_cbc(ctx->cipher_ctx.cipher_ctx, ctx->cipher_ctx.operation, cbc_length, cbc_iv, b_input, b_output);

    get_ctr_iv(ctr_iv, iv, iv_len);

    get_ctr_enc_input(length, input);

    ctr_length = 16 * ctr_pt_round;
    arch_memset(b_output, 0, sizeof(b_output));

    /*reset this value for init ctr and set new IV*/
    aes_ctx.nr = 0;

    mbedtls_aes_crypt_ctr(ctx->cipher_ctx.cipher_ctx, ctr_length, &nc_off, ctr_iv, stream_block, b_input, b_output);

    /*first output block is tag info*/
    arch_memcpy(tag, b_output, tag_len);

    /*second ~ ? output block is pt info*/
    arch_memcpy(output, b_output + 16, length);

    return (0);
}

int mbedtls_ccm_auth_decrypt(mbedtls_ccm_context *ctx, size_t length, const unsigned char *iv, size_t iv_len, const unsigned char *add,
                             size_t add_len, const unsigned char *input, unsigned char *output, const unsigned char *tag, size_t tag_len)
{
    size_t ctr_length = 0;
    static unsigned char cbc_mac[16] = { 0 };
    size_t nc_off = 0;
    unsigned char stream_block[16] = { 0 };
    unsigned char cbc_iv[16] = { 0 };
    unsigned char ctr_iv[16] = { 0 };
    size_t cbc_length = 0;
    int ret = 0;

    /* ctr part*/
    get_ctr_iv(ctr_iv, iv, iv_len);

    get_ctr_dec_input(length, input, tag, tag_len);

    arch_memset(b_output, 0, sizeof(b_output));

    ctr_length = 16 * ctr_ct_round;

    /*reset this value for init ctr and set new IV*/
    aes_ctx.nr = 0;
    mbedtls_aes_crypt_ctr(ctx->cipher_ctx.cipher_ctx, ctr_length, &nc_off, ctr_iv, stream_block, b_input, b_output);

    /* Copy CBC-MAC here */
    arch_memcpy(cbc_mac, b_output, tag_len);

    /* Copy plaintext here */
    arch_memcpy(output, b_output + 16, length);

    /* CBC-MAC part*/

    ret = get_cbc_mac_input(ctx, length, iv, iv_len, add, add_len, output, tag_len);

    if (ret) {
        return (ret);
    }

    cbc_length = 16 * cbc_pt_round;

    arch_memset(b_output, 0, sizeof(b_output));

    aes_ctx.nr = 0;
    mbedtls_aes_crypt_cbc(ctx->cipher_ctx.cipher_ctx, ctx->cipher_ctx.operation, cbc_length, cbc_iv, b_input, b_output);

    /* Check CBC-MAC */
    if (memcmp(cbc_mac, b_output + (cbc_pt_round - 1) * 16, tag_len) != 0) {
        return (-1);
    }

    return (ret);
}
