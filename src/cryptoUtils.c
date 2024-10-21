#include "include/cryptoUtils.h"
#include  <string.h>


const EVP_CIPHER *modeForAES128(ENC_MODE mode);
const EVP_CIPHER *modeForAES192(ENC_MODE mode);
const EVP_CIPHER *modeForAES256(ENC_MODE mode);
const EVP_CIPHER *modeFor3DES(ENC_MODE mode);

const EVP_CIPHER *determineCipherAndMode(ENCRYPTION encryption, ENC_MODE mode)
{
    switch (encryption)
    {
        case AES128:
            return modeForAES128(mode);
        case AES192:
            return modeForAES192(mode);
        case AES256:
            return modeForAES256(mode);
        case ENC_3DES:
            return modeFor3DES(mode);
        case ENC_NONE:
        default:
                return NULL;
    }
}

const EVP_CIPHER *modeForAES128(ENC_MODE mode){
    switch (mode){
        case ECB:
            return EVP_aes_128_ecb();
        case CFB:
            return EVP_aes_128_cfb1();
        case OFB:
            return EVP_aes_128_ofb();
        case CBC:
            return EVP_aes_128_cbc();
        default:
            break;
    }

    return NULL;
}

const EVP_CIPHER *modeForAES192(ENC_MODE mode){

     switch (mode){
        case ECB:
            return EVP_aes_192_ecb();
        case CFB:
            return EVP_aes_192_cfb1();
        case OFB:
            return EVP_aes_192_ofb();
        case CBC:
            return EVP_aes_192_cbc();
        default:
            break;
    }

    return NULL;
}

const EVP_CIPHER *modeForAES256(ENC_MODE mode)
{
    switch (mode)
    {
        case ECB:
            return EVP_aes_256_ecb();
        case CFB:
            return EVP_aes_256_cfb1();
        case OFB:
            return EVP_aes_256_ofb();
        case CBC:
            return EVP_aes_256_cbc();
        default:
            break;
    }

    return NULL;
}

const EVP_CIPHER *modeFor3DES(ENC_MODE mode)
{
    switch (mode)
    {
        case ECB:
            return EVP_des_ecb();
        case CFB:
            return EVP_des_cfb1();
        case OFB:
            return EVP_des_ofb();
        case CBC:
            return EVP_des_cbc();
        default:
            break;
    }

    return NULL;
}

size_t determineKeyLength(ENCRYPTION encryption)
{
    switch (encryption)
    {
        case AES128:
            return 16;
        case AES192:
            return 24;
        case AES256:
            return 32;
        case ENC_3DES:
            return 8;
        case ENC_NONE:
        default:
            return 0;
    }
}

void failureMSG(){
    fprintf(stderr, "Failed to encrypt data\n");
    exit(0);
}

void failureDEC() {
    fprintf(stderr, "Failed to decrypt data\n");
    exit(0);
}

int encrypt(const uint8_t *plaintext, int ptextLen, uint8_t *ciphertext, ENCRYPTION encryption, ENC_MODE mode,const uint8_t *password){
    EVP_CIPHER_CTX *ctx;
    int auxLen, ciphertextLen;
    const EVP_CIPHER *cipher = determineCipherAndMode(encryption, mode);
    size_t keyLen = determineKeyLength(encryption);
    uint8_t *key = malloc(keyLen);
    uint8_t *iv = malloc(keyLen);
    EVP_BytesToKey(cipher, EVP_sha256(), NULL, password, (int)strlen((char *)password), 1, key, iv);

    if (!(ctx = EVP_CIPHER_CTX_new()))
        failureMSG();

    if (EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv) != 1)
        failureMSG();

    if (EVP_EncryptUpdate(ctx, ciphertext, &auxLen, plaintext, ptextLen) != 1)
        failureMSG();

    ciphertextLen = auxLen;

    if (EVP_EncryptFinal_ex(ctx, ciphertext + auxLen, &auxLen) != 1)
        failureMSG();

    ciphertextLen += auxLen;
    EVP_CIPHER_CTX_free(ctx);

    free(key);
    free(iv);

    return ciphertextLen;
}

uint8_t *decrypt(const ENC_MESSAGE *encMsg, ENCRYPTION encryption, ENC_MODE mode, const uint8_t *password){
    EVP_CIPHER_CTX *ctx;
    int auxLen;
    const EVP_CIPHER *cipher = determineCipherAndMode(encryption, mode);
    uint8_t *plaintext = calloc(encMsg->size, 1);
    size_t keyLen = determineKeyLength(encryption);
    uint8_t *key = malloc(keyLen);
    uint8_t *iv = malloc(keyLen);

    EVP_BytesToKey(cipher, EVP_sha256(), NULL, password, (int)strlen((char *)password), 1, key, iv);

    if (!(ctx = EVP_CIPHER_CTX_new()))
        failureDEC();

    if (EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv) != 1)
        failureDEC();

    if (EVP_DecryptUpdate(ctx, plaintext, &auxLen, encMsg->data, encMsg->size) != 1)
        failureDEC();

    if (EVP_DecryptFinal_ex(ctx, plaintext + auxLen, &auxLen) != 1)
        failureDEC();

    EVP_CIPHER_CTX_free(ctx);

    free(key);
    free(iv);

    return plaintext;
}
