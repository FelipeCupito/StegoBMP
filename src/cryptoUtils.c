#include "include/cryptoUtils.h"
#include  <string.h>


const EVP_CIPHER *modeForAES128(EncryptionMode mode);
const EVP_CIPHER *modeForAES192(EncryptionMode mode);
const EVP_CIPHER *modeForAES256(EncryptionMode mode);
const EVP_CIPHER *modeFor3DES(EncryptionMode mode);

const EVP_CIPHER *determineCipherAndMode(EncryptionAlgorithm encryption, EncryptionMode mode)
{
    switch (encryption)
    {
        case ENC_AES128:
            return modeForAES128(mode);
        case ENC_AES192:
            return modeForAES192(mode);
        case ENC_AES256:
            return modeForAES256(mode);
        case ENC_3DES:
            return modeFor3DES(mode);
        case ENC_NONE:
        default:
                return NULL;
    }
}

const EVP_CIPHER *modeForAES128(EncryptionMode mode){
    switch (mode){
        case ENC_MODE_ECB:
            return EVP_aes_128_ecb();
        case ENC_MODE_CFB:
            return EVP_aes_128_cfb1();
        case ENC_MODE_OFB:
            return EVP_aes_128_ofb();
        case ENC_MODE_CBC:
            return EVP_aes_128_cbc();
        default:
            break;
    }

    return NULL;
}

const EVP_CIPHER *modeForAES192(EncryptionMode mode){

     switch (mode){
        case ENC_MODE_ECB:
            return EVP_aes_192_ecb();
        case ENC_MODE_CFB:
            return EVP_aes_192_cfb1();
        case ENC_MODE_OFB:
            return EVP_aes_192_ofb();
        case ENC_MODE_CBC:
            return EVP_aes_192_cbc();
        default:
            break;
    }

    return NULL;
}

const EVP_CIPHER *modeForAES256(EncryptionMode mode)
{
    switch (mode)
    {
        case ENC_MODE_ECB:
            return EVP_aes_256_ecb();
        case ENC_MODE_CFB:
            return EVP_aes_256_cfb1();
        case ENC_MODE_OFB:
            return EVP_aes_256_ofb();
        case ENC_MODE_CBC:
            return EVP_aes_256_cbc();
        default:
            break;
    }

    return NULL;
}

const EVP_CIPHER *modeFor3DES(EncryptionMode mode)
{
    switch (mode)
    {
        case ENC_MODE_ECB:
            return EVP_des_ecb();
        case ENC_MODE_CFB:
            return EVP_des_cfb1();
        case ENC_MODE_OFB:
            return EVP_des_ofb();
        case ENC_MODE_CBC:
            return EVP_des_cbc();
        default:
            break;
    }

    return NULL;
}

size_t determineKeyLength(EncryptionAlgorithm encryption)
{
    switch (encryption)
    {
        case ENC_AES128:
            return 16;
        case ENC_AES192:
            return 24;
        case ENC_AES256:
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

ENC_MESSAGE* encrypt(const char* plaintext, int ptextLen, EncryptionAlgorithm encryption, EncryptionMode mode,const uint8_t *password){
    EVP_CIPHER_CTX *ctx;
    int auxLen, ciphertextLen;
    uint8_t *ciphertext;
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
    //TODO sacar ptextLen y cambiar por strlen
    ENC_MESSAGE *ans = malloc(sizeof(ENC_MESSAGE));
    ans->data = ciphertext;
    ans->size = ciphertextLen;
    return ans;
}

uint8_t *decryptAUX(const ENC_MESSAGE *encMsg, EncryptionAlgorithm encryption, EncryptionMode mode, const uint8_t *password){
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
    //SIZE+EXTENSION+LA INFO
}

ENC_MESSAGE *cryto(const FilePackage* filePackage, EncryptionAlgorithm encryption, EncryptionMode mode, const uint8_t *password) {
    size_t total_len = filePackage->size + strlen((char *)filePackage->data) + strlen(filePackage->extension) + 20; // 20 extra for size_t conversion
    char *ptext = malloc(total_len);
    sprintf(ptext, "%zu%s%s", filePackage->size, filePackage->data, filePackage->extension);
    ENC_MESSAGE *ans = encrypt(ptext,(int)strlen(ptext),encryption,mode,password);
    free(ptext);
    return ans;
}
FilePackage *decrypt(ENC_MESSAGE *encMsg,EncryptionAlgorithm encryption, EncryptionMode mode, const uint8_t *password) {
    uint8_t *text = decryptAUX(encMsg,encryption,mode,password);
    FilePackage *filePackage = malloc(sizeof(FilePackage));
    return filePackage;
}
