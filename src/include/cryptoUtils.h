#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include <file_handler.h>
#include <openssl/evp.h>
#include <stdint.h>
#include "types.h"

typedef struct
{
    uint32_t size;
    uint8_t *data;
} ENC_MESSAGE;

ENC_MESSAGE *cryto(const FilePackage* filePackage, EncryptionAlgorithm encryption, EncryptionMode mode, const uint8_t *password);
FilePackage *decrypt(ENC_MESSAGE *encMsg,EncryptionAlgorithm encryption, EncryptionMode mode, const uint8_t *password);

#endif