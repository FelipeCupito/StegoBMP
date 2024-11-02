#ifndef EMBED_H
#define EMBED_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "bmp_image.h"
#include "file_package.h"
#include "logger.h"
#include "types.h"

#define IS_DATA_BIG_ENDIAN true   // Cambia esta macro según el formato de los datos
#define BYTES_TO_BITS(bytes) ((bytes) * 8) // Convierte bytes a bits
#define IS_SYSTEM_BIG_ENDIAN() ( (*(uint16_t*)"\0\xff" < 0x100) )

/**
 * @brief Inserta datos secretos en una imagen BMP utilizando el algoritmo de esteganografía especificado.
 *
 * @param bmp         Puntero a la estructura BMPImage donde se insertarán los datos.
 * @param secret_data Puntero a los datos secretos que se desean insertar.
 * @param secret_size Tamaño de los datos secretos en bytes.
 * @param steg_alg    Algoritmo de esteganografía a utilizar (STEG_LSB1, STEG_LSB4, STEG_LSBI).
 * @return bool       true si la inserción fue exitosa, false en caso de error.
 */
bool embed(BMPImage *bmp, const uint8_t *secret_data, size_t secret_size, StegAlgorithm steg_alg);

/**
 * @brief Extrae datos ocultos (tamaño, datos y extensión) de una imagen BMP utilizando el algoritmo especificado.
 *
 * @param bmp        Puntero a la estructura BMPImage de donde se extraerán los datos.
 * @param steg_alg   Algoritmo de esteganografía a utilizar para la extracción (STEG_LSB1, STEG_LSB4, STEG_LSBI).
 * @return FilePackage*  Puntero a una estructura FilePackage con los datos extraídos, o NULL en caso de error.
 */
FilePackage* extract_data(const BMPImage *bmp, StegAlgorithm steg_alg);

/**
 * @brief Extrae datos ocultos encriptados (tamaño cifrado y datos cifrados) de una imagen BMP utilizando el algoritmo especificado.
 *
 * @param bmp             Puntero a la estructura BMPImage de donde se extraerán los datos.
 * @param steg_alg        Algoritmo de esteganografía a utilizar para la extracción (STEG_LSB1, STEG_LSB4, STEG_LSBI).
 * @param extracted_size  Puntero donde se almacenará el tamaño de los datos extraídos.
 * @return uint8_t*       Puntero al buffer que contiene los datos extraídos, o NULL en caso de error.
 *                        El llamante es responsable de liberar la memoria.
 */
uint8_t* extract_encrypted_data(const BMPImage *bmp, StegAlgorithm steg_alg, size_t *extracted_size);

#ifdef TESTING
/**
 * Only used for testing purposes.
 */
bool embed_bits_lsb1(BMPImage *bmp, const uint8_t *data, size_t num_bits, size_t *offset);
bool embed_bits_lsb4(BMPImage *bmp, const uint8_t *data, size_t num_bits, size_t *offset);
bool embed_bits_lsbi(BMPImage *bmp, const uint8_t *data, size_t num_bits, size_t *offset);
bool extract_bits_lsb1(const BMPImage *bmp, size_t num_bits, uint8_t *buffer, size_t *offset, void *context);
bool extract_bits_lsb4(const BMPImage *bmp, size_t num_bits, uint8_t *buffer, size_t *offset, void *context);
bool extract_bits_lsbi(const BMPImage *bmp, size_t num_bits, uint8_t *buffer, size_t *offset, void *context);
bool check_capacity_lsb1(const BMPImage *bmp, size_t num_bits);
bool check_capacity_lsb4(const BMPImage *bmp, size_t num_bits);
bool check_capacity_lsbi(const BMPImage *bmp, size_t num_bits);

bool embed_bits_generic(BMPImage *bmp, const uint8_t *data, size_t num_bits, size_t *offset, int bits_per_component);
bool extract_bits_generic(const BMPImage *bmp, size_t num_bits, uint8_t *buffer, size_t *offset, int bits_per_component);

size_t extract_data_size(const BMPImage *bmp, StegAlgorithm steg_alg, size_t *offset, void *context);
void format_data_endian(uint8_t *data, size_t size);
bool extract_extension(const BMPImage *bmp, StegAlgorithm steg_alg, char *ext_buffer, size_t *offset, void *context);
#endif

#endif //EMBED_H