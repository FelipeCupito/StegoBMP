#ifndef STEGOBMP_TYPES_H
#define STEGOBMP_TYPES_H

#include <string.h>
#include <stdio.h>

#define SUCCEEDED 1
#define FAILED    0

typedef enum {
    MODE_NONE,
    MODE_EMBED,
    MODE_EXTRACT
} OperationMode;

typedef enum {
    STEG_NONE,
    STEG_LSB1,
    STEG_LSB4,
    STEG_LSBI
} StegAlgorithm;

typedef enum {
    ENC_NONE,
    ENC_AES128,
    ENC_AES192,
    ENC_AES256,
    ENC_3DES
} EncryptionAlgorithm;

typedef enum {
    ENC_MODE_NONE,
    ENC_MODE_ECB,
    ENC_MODE_CFB,
    ENC_MODE_OFB,
    ENC_MODE_CBC
} EncryptionMode;

#endif //STEGOBMP_TYPES_H
