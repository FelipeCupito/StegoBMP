#ifndef STEGOBMP_UTILS_H
#define STEGOBMP_UTILS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BYTES_TO_BITS(bytes) ((bytes) * 8)
#define EXTENSION_SIZE 16

#define IS_DATA_BIG_ENDIAN true                         // Es el endianess en que se embeben los datos y se espera que estén almacenados.
#define IS_SYSTEM_BIG_ENDIAN() (is_system_big_endian()) // Es el endianess del sistema en que se ejecuta el programa.

/**
* @brief Ajusta la endianess de un buffer de datos de 4 bytes según el sistema.
*
* Esta función verifica si el sistema y los datos almacenados tienen la misma endianess
* (es decir, si son ambos big-endian o little-endian). Si hay una discrepancia entre la
* endianess del sistema y la de los datos, la función invierte el orden de los bytes en el
* buffer para ajustarlo a la endianess del sistema.
*
* @param buffer Puntero al buffer de datos de 4 bytes que será ajustado en caso de discrepancia
*               de endianess. Se espera que el buffer contenga exactamente 4 bytes de datos.
*/
void adjust_data_endianness(uint8_t *buffer);

/**
 * @brief Obtiene el tamaño de un archivo.
 *
 * @param file Puntero al archivo abierto en modo binario.
 * @return size_t Tamaño del archivo en bytes, o 0 si ocurre un error.
 */
size_t get_file_size(FILE *file);

/**
 * @brief Obtiene la extensión de un archivo a partir de su nombre.
 *
 * @param file_name Nombre del archivo.
 * @return uint8_t* Cadena duplicada con la extensión del archivo (incluyendo el punto), o NULL si no se encuentra una extensión válida.
 *                 El llamante es responsable de liberar la memoria.
 */
uint8_t* get_file_extension(const char *file_name);

/**
 * @brief Detecta si el sistema es big-endian.
 *
 * @return true si el sistema es big-endian, false si es little-endian.
 */
static bool is_system_big_endian() {
    uint16_t test = 0x1;
    return ((*(uint8_t *)&test) == 0) ? true : false;
}

#endif // STEGOBMP_UTILS_H
