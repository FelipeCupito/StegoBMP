#ifndef STEGOBMP_UTILS_H
#define STEGOBMP_UTILS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define IS_DATA_BIG_ENDIAN true   // Cambia esta macro según el formato de los datos
#define BYTES_TO_BITS(bytes) ((bytes) * 8)
#define IS_SYSTEM_BIG_ENDIAN() (is_system_big_endian())

#define EXTENSION_SIZE 16

/**
 * @brief Ajusta la representación endian de un valor de 32 bits y lo almacena en un buffer.
 *
 * Esta función toma un valor de 32 bits almacenado en `size` y lo convierte
 * al formato endian correcto según el sistema y la configuración de los datos.
 * Si el sistema y los datos tienen un endianess diferente, la función ajusta el valor
 * y lo almacena en `buffer` en el orden apropiado.
 *
 * @param buffer    Puntero al buffer donde se almacenará el valor convertido en formato endian.
 *                  El buffer debe tener al menos 4 bytes.
 * @param size      El valor de 32 bits a convertir según el endianess.
 */
void adjust_data_endianness(uint8_t *buffer, uint32_t size);

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

///**
// * @brief Formatea un valor de 32 bits a la endianess deseada y lo almacena en un buffer.
// *
// * Similar a `adjust_data_endianness`, pero separa la lógica de lectura y escritura.
// *
// * @param value  El valor de 32 bits a formatear.
// * @param buffer Puntero al buffer donde se almacenará el valor formateado.
// */
//void format_data_endian(uint32_t value, uint8_t *buffer);

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
