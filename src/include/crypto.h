#ifndef STEGOBMP_CRYPTO_H
#define STEGOBMP_CRYPTO_H

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <openssl/err.h>
#include <openssl/evp.h>

#include "logger.h"
#include "bmp_image.h"
#include "types.h"
#include "file_package.h"

/**
 * @brief Encripta datos utilizando el algoritmo y modo especificado.
 *
 * @param data           Puntero a los datos a encriptar.
 * @param size           Tamaño de los datos a encriptar en bytes.
 * @param encryption     Algoritmo de encriptación a utilizar (e.g., ENC_AES128, ENC_AES192, ENC_AES256, ENC_3DES).
 * @param mode           Modo de encriptación a utilizar (e.g., ENC_MODE_ECB, ENC_MODE_CFB, ENC_MODE_OFB, ENC_MODE_CBC).
 * @param password       Contraseña para generar la clave y el IV.
 * @param encrypted_size Puntero donde se almacenará el tamaño de los datos encriptados.
 * @return uint8_t*      Puntero a los datos encriptados. El llamante es responsable de liberar la memoria.
 *                       Devuelve NULL en caso de error.
 */
uint8_t* crypto_encrypt(const uint8_t *data, size_t size, EncryptionAlgorithm encryption, EncryptionMode mode, const char *password, size_t *encrypted_size);

/**
 * @brief Desencripta datos utilizando el algoritmo y modo especificado.
 *
 * @param encrypted_data Puntero a los datos encriptados.
 * @param encrypted_size Tamaño de los datos encriptados en bytes.
 * @param encryption     Algoritmo de desencriptación a utilizar (debe coincidir con el utilizado en la encriptación).
 * @param mode           Modo de desencriptación a utilizar (debe coincidir con el utilizado en la encriptación).
 * @param password       Contraseña utilizada para generar la clave y el IV.
 * @param decrypted_size Puntero donde se almacenará el tamaño de los datos desencriptados.
 * @return uint8_t*      Puntero a los datos desencriptados. El llamante es responsable de liberar la memoria.
 *                       Devuelve NULL en caso de error.
 */
uint8_t* crypto_decrypt(const uint8_t *encrypted_data, size_t encrypted_size, EncryptionAlgorithm encryption, EncryptionMode mode, const uint8_t *password, size_t *decrypted_size);

#endif