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

typedef enum ENCRYPTION{
    ENC_NONE,
    AES128,
    AES192,
    AES256,
    ENC_3DES
} ENCRYPTION;

typedef enum ENC_MODE{
    ENC_MODE_NONE,
    ECB,
    CFB,
    OFB,
    CBC
} ENC_MODE;

#endif //STEGOBMP_TYPES_H
