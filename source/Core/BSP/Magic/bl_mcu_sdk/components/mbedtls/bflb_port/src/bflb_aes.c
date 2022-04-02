#include "mbedtls/aes.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls_port_config.h"

static SEC_ENG_AES_Key_Type get_key_type(uint32_t keybits)
{
    switch (keybits) {
        case 192:
            return SEC_ENG_AES_KEY_192BITS;

        case 256:
            return SEC_ENG_AES_KEY_256BITS;

        default:
            return SEC_ENG_AES_KEY_128BITS;
    }
}

void mbedtls_aes_init(mbedtls_aes_context *ctx)
{
    CHECK_PARAM(ctx != NULL);
    Sec_Eng_AES_Enable_BE(SEC_ENG_AES_ID0);
    arch_memset(ctx, 0, sizeof(mbedtls_aes_context));
}

void mbedtls_aes_free(mbedtls_aes_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    arch_memset(ctx, 0, sizeof(mbedtls_aes_context));
}

int mbedtls_aes_setkey_enc(mbedtls_aes_context *ctx, const unsigned char *key, unsigned int keybits)
{
    ctx->nr = 0;
    ctx->rk = (uint32_t *)key;
    ctx->buf[0] = keybits;

    return (0);
}

int mbedtls_aes_setkey_dec(mbedtls_aes_context *ctx, const unsigned char *key, unsigned int keybits)
{
    ctx->nr = 0;
    ctx->rk = (uint32_t *)key;
    ctx->buf[0] = keybits;

    return (0);
}

int mbedtls_aes_crypt_cbc(mbedtls_aes_context *ctx, int mode, size_t length, unsigned char iv[16], const unsigned char *input, unsigned char *output)
{
    SEC_Eng_AES_Ctx aesCtx;

    if (!ctx->nr) {
        Sec_Eng_AES_Init(&aesCtx, SEC_ENG_AES_ID0, SEC_ENG_AES_CBC, get_key_type(ctx->buf[0]), (SEC_ENG_AES_EnDec_Type)(~mode & 1));
        Sec_Eng_AES_Set_Key_IV_BE(SEC_ENG_AES_ID0, SEC_ENG_AES_KEY_SW, (uint8_t *)ctx->rk, iv);
    }

    aesCtx.aesFeed = ctx->nr;
    aesCtx.mode = SEC_ENG_AES_CBC;
    Sec_Eng_AES_Crypt(&aesCtx, SEC_ENG_AES_ID0, input, length, output);
    ctx->nr = 1;

    return (0);
}

int mbedtls_aes_crypt_ecb(mbedtls_aes_context *ctx, int mode, const unsigned char input[16], unsigned char output[16])
{
    SEC_Eng_AES_Ctx aesCtx;
    const uint8_t iv[16] = { 0 };

    if (!ctx->nr) {
        Sec_Eng_AES_Init(&aesCtx, SEC_ENG_AES_ID0, SEC_ENG_AES_ECB, get_key_type(ctx->buf[0]), (SEC_ENG_AES_EnDec_Type)(~mode & 1));
        Sec_Eng_AES_Set_Key_IV_BE(SEC_ENG_AES_ID0, SEC_ENG_AES_KEY_SW, (uint8_t *)ctx->rk, iv);
    }

    aesCtx.aesFeed = ctx->nr;
    aesCtx.mode = SEC_ENG_AES_ECB;
    Sec_Eng_AES_Crypt(&aesCtx, SEC_ENG_AES_ID0, input, 16, output);
    ctx->nr = 1;

    return (0);
}

int mbedtls_aes_crypt_ctr(mbedtls_aes_context *ctx, size_t length, size_t *nc_off, unsigned char nonce_counter[16],
                          unsigned char stream_block[16], const unsigned char *input, unsigned char *output)
{
    SEC_Eng_AES_Ctx aesCtx;

    if (!ctx->nr) {
        // *(uint32_t *)(nonce_counter + 12) += *nc_off;
        Sec_Eng_AES_Init(&aesCtx, SEC_ENG_AES_ID0, SEC_ENG_AES_CTR, get_key_type(ctx->buf[0]), SEC_ENG_AES_ENCRYPTION);
        Sec_Eng_AES_Set_Key_IV_BE(SEC_ENG_AES_ID0, SEC_ENG_AES_KEY_SW, (uint8_t *)ctx->rk, nonce_counter);
    }

    aesCtx.aesFeed = ctx->nr;
    aesCtx.mode = SEC_ENG_AES_CTR;
    Sec_Eng_AES_Crypt(&aesCtx, SEC_ENG_AES_ID0, input, length, output);
    ctx->nr = 1;

    return (0);
}
