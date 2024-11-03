#include "crypto.h"


/**
 * @brief Obtiene el cipher de OpenSSL correspondiente al algoritmo y modo especificados.
 *
 * @param encryption Algoritmo de encriptación.
 * @param mode       Modo de encriptación.
 * @return const EVP_CIPHER* Cipher de OpenSSL correspondiente, o NULL si no se encuentra.
 */
const EVP_CIPHER* determine_cipher(EncryptionAlgorithm encryption, EncryptionMode mode) {
    switch (encryption) {
        case ENC_AES128:
            switch (mode) {
                case ENC_MODE_ECB:
                    return EVP_aes_128_ecb();
                case ENC_MODE_CFB:
                    return EVP_aes_128_cfb1();
                case ENC_MODE_OFB:
                    return EVP_aes_128_ofb();
                case ENC_MODE_CBC:
                    return EVP_aes_128_cbc();
                default:
                    return NULL;
            }
        case ENC_AES192:
            switch (mode) {
                case ENC_MODE_ECB:
                    return EVP_aes_192_ecb();
                case ENC_MODE_CFB:
                    return EVP_aes_192_cfb1();
                case ENC_MODE_OFB:
                    return EVP_aes_192_ofb();
                case ENC_MODE_CBC:
                    return EVP_aes_192_cbc();
                default:
                    return NULL;
            }
        case ENC_AES256:
            switch (mode) {
                case ENC_MODE_ECB:
                    return EVP_aes_256_ecb();
                case ENC_MODE_CFB:
                    return EVP_aes_256_cfb1();
                case ENC_MODE_OFB:
                    return EVP_aes_256_ofb();
                case ENC_MODE_CBC:
                    return EVP_aes_256_cbc();
                default:
                    return NULL;
            }
        case ENC_3DES:
            switch (mode) {
                case ENC_MODE_ECB:
                    return EVP_des_ecb();
                case ENC_MODE_CFB:
                    return EVP_des_ede3_cfb8();
                case ENC_MODE_OFB:
                    return EVP_des_ofb();
                case ENC_MODE_CBC:
                    return EVP_des_cbc();
                default:
                    return NULL;
            }
        case ENC_NONE:
        default:
            return NULL;
    }
}

/**
 * @brief Maneja errores de OpenSSL.
 *
 * @param msg Mensaje de error a mostrar.
 */
void handle_openssl_error(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
}

//TOD0: revisar si size y encrypted_size pueden ser el mismo argumento
uint8_t* crypto_encrypt(const uint8_t *data, size_t size, EncryptionAlgorithm encryption, EncryptionMode mode, const char *password, size_t *encrypted_size) {
    if (!data || size == 0 || !password || !encrypted_size) {
        LOG(ERROR, "Invalid arguments to crypto_encrypt.")
        return NULL;
    }

    const EVP_CIPHER *cipher = determine_cipher(encryption, mode);
    if (!cipher) {
        LOG(ERROR, "Unsupported encryption algorithm or mode.")
        return NULL;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        handle_openssl_error("Failed to create EVP_CIPHER_CTX.");
    }

    // Generar clave e IV usando PBKDF2
    int key_len = EVP_CIPHER_key_length(cipher);
    int iv_len = EVP_CIPHER_iv_length(cipher);
    int total_len = key_len + iv_len;
    unsigned char *key_iv = malloc(total_len);
    if (!key_iv) {
        LOG(ERROR, "Memory allocation failed for key_iv.")
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    // Sal (salt) fija de 8 bytes (puedes mejorar esto usando una sal aleatoria)
    unsigned char salt[8] = {0};
    if (!PKCS5_PBKDF2_HMAC((const char *)password, strlen((const char *)password), salt, sizeof(salt), 10000, EVP_sha256(), total_len, key_iv)) {
        handle_openssl_error("PKCS5_PBKDF2_HMAC failed.");
    }

    unsigned char *key = key_iv;
    unsigned char *iv = key_iv + key_len;

    // Inicializar la encriptación
    if (EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv) != 1) {
        handle_openssl_error("EVP_EncryptInit_ex failed.");
    }

    //TODO: no se que es lo agrego gpt, revisar que es
    // EVP_CIPHER_CTX_set_padding(ctx, 0);

    // Asignar memoria para el ciphertext (size + block_size)
    int block_size = EVP_CIPHER_block_size(cipher);
    int max_ciphertext_len = size + block_size;
    unsigned char *ciphertext = malloc(max_ciphertext_len);
    if (!ciphertext) {
        LOG(ERROR, "Memory allocation failed for ciphertext.")
        EVP_CIPHER_CTX_free(ctx);
        free(key_iv);
        return NULL;
    }

    int len;
    int ciphertext_len = 0;

    // Encriptar los datos
    if (EVP_EncryptUpdate(ctx, ciphertext, &len, data, size) != 1) {
        handle_openssl_error("EVP_EncryptUpdate failed.");
    }
    ciphertext_len += len;

    // Finalizar la encriptación
    if (EVP_EncryptFinal_ex(ctx, ciphertext + ciphertext_len, &len) != 1) {
        handle_openssl_error("EVP_EncryptFinal_ex failed.");
    }
    ciphertext_len += len;

    // Limpiar
    EVP_CIPHER_CTX_free(ctx);
    free(key_iv);

    *encrypted_size = ciphertext_len;
    return ciphertext;
}

uint8_t* crypto_decrypt(const uint8_t *encrypted_data, size_t encrypted_size, EncryptionAlgorithm encryption, EncryptionMode mode, const uint8_t *password, size_t *decrypted_size) {
    if (!encrypted_data || encrypted_size == 0 || !password || !decrypted_size) {
        LOG(ERROR, "Invalid arguments to crypto_decrypt.")
        return NULL;
    }

    const EVP_CIPHER *cipher = determine_cipher(encryption, mode);
    if (!cipher) {
        LOG(ERROR, "Unsupported decryption algorithm or mode.")
        return NULL;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        handle_openssl_error("Failed to create EVP_CIPHER_CTX.");
    }

    // Generar clave e IV usando PBKDF2 (debe ser el mismo que en la encriptación)
    int key_len = EVP_CIPHER_key_length(cipher);
    int iv_len = EVP_CIPHER_iv_length(cipher);
    int total_len = key_len + iv_len;
    unsigned char *key_iv = malloc(total_len);
    if (!key_iv) {
        LOG(ERROR, "Memory allocation failed for key_iv.")
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    // Sal (salt) fija de 8 bytes (debe ser la misma que en la encriptación)
    unsigned char salt[8] = {0};
    if (!PKCS5_PBKDF2_HMAC((const char *)password, strlen((const char *)password), salt, sizeof(salt), 10000, EVP_sha256(), total_len, key_iv)) {
        handle_openssl_error("PKCS5_PBKDF2_HMAC failed.");
    }

    unsigned char *key = key_iv;
    unsigned char *iv = key_iv + key_len;

    // Inicializar la desencriptación
    if (EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv) != 1) {
        handle_openssl_error("EVP_DecryptInit_ex failed.");
    }

    // Opcional: Deshabilitar padding si no es necesario
    // EVP_CIPHER_CTX_set_padding(ctx, 0);

    // Asignar memoria para el plaintext (encrypted_size)
    unsigned char *plaintext = malloc(encrypted_size);
    if (!plaintext) {
        LOG(ERROR, "Memory allocation failed for plaintext.")
        EVP_CIPHER_CTX_free(ctx);
        free(key_iv);
        return NULL;
    }

    int len;
    int plaintext_len = 0;

    // Desencriptar los datos
    if (EVP_DecryptUpdate(ctx, plaintext, &len, encrypted_data, encrypted_size) != 1) {
        handle_openssl_error("EVP_DecryptUpdate failed.");
    }
    plaintext_len += len;

    // Finalizar la desencriptación
    if (EVP_DecryptFinal_ex(ctx, plaintext + plaintext_len, &len) != 1) {
        handle_openssl_error("EVP_DecryptFinal_ex failed.");
    }
    plaintext_len += len;

    // Limpiar
    EVP_CIPHER_CTX_free(ctx);
    free(key_iv);

    *decrypted_size = plaintext_len;
    return plaintext;
}
