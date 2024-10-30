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
            return EVP_aes_256_cfb8();
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
            return EVP_des_ede3_cfb8();
        case ENC_MODE_OFB:
            return EVP_des_ofb();
        case ENC_MODE_CBC:
            return EVP_des_cbc();
        default:
            break;
    }

    return NULL;
}


void failureMSG(){
    fprintf(stderr, "Failed to encrypt data\n");
    exit(0);
}

void failureDEC() {
    fprintf(stderr, "Failed to decrypt data\n");
    exit(0);
}

ENC_MESSAGE* encrypt(const char* plaintext,EncryptionAlgorithm encryption, EncryptionMode mode,const char *password){
    EVP_CIPHER_CTX *ctx;
    int auxLen, ciphertextLen;
    uint8_t *ciphertext;
    const EVP_CIPHER *cipher = determineCipherAndMode(encryption, mode);
    const int keylen = EVP_CIPHER_key_length(cipher);
    const int ivlen = EVP_CIPHER_iv_length(cipher);
    unsigned char* key_iv_pair = malloc(keylen + ivlen);
    const unsigned char salt[8] = {0};
    PKCS5_PBKDF2_HMAC(password,-1,salt,sizeof(salt),10000,EVP_sha256(),keylen+ivlen,key_iv_pair);

    if (!(ctx = EVP_CIPHER_CTX_new()))
        failureMSG();

    if (EVP_EncryptInit_ex2(ctx, cipher, key_iv_pair, key_iv_pair + keylen,NULL) != 1)
        failureMSG();

    if (EVP_EncryptUpdate(ctx, ciphertext, &auxLen, plaintext, strlen(plaintext)) != 1)
        failureMSG();

    ciphertextLen = auxLen;

    if (EVP_EncryptFinal_ex(ctx, ciphertext + auxLen, &auxLen) != 1)
        failureMSG();

    ciphertextLen += auxLen;
    EVP_CIPHER_CTX_free(ctx);

    free(key_iv_pair);
    EVP_CIPHER_CTX_free(ctx);
    EVP_CIPHER_free(cipher);
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
    size_t keyLen = EVP_CIPHER_key_length(cipher);
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
    ENC_MESSAGE *ans = encrypt(ptext,encryption,mode,password);
    free(ptext);
    return ans;
}
FilePackage *decrypt(ENC_MESSAGE *encMsg,EncryptionAlgorithm encryption, EncryptionMode mode, const uint8_t *password) {
    uint8_t *text = decryptAUX(encMsg,encryption,mode,password);
    return new_file_package_from_data(text);
}
