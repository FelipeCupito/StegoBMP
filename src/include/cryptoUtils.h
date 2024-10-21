#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include <openssl/evp.h>
#include <stdint.h>
#include "types.h"

typedef struct
{
    uint32_t size;
    uint8_t *data;
} ENC_MESSAGE;

int encryptData(const uint8_t *plaintext, int ptextLen, uint8_t *ciphertext, ENCRYPTION encryption, ENC_MODE mode, const uint8_t *password);
uint8_t *decryptData(const ENC_MESSAGE *encMsg, ENCRYPTION encryption, ENC_MODE mode, const uint8_t *password);

#endif