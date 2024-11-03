#include "utils.h"
#include "logger.h"

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
void adjust_data_endianness(uint8_t *buffer, uint32_t size) {
    // Verificar si el endianess del sistema difiere del endianess de los datos.
    if (IS_SYSTEM_BIG_ENDIAN() != IS_DATA_BIG_ENDIAN) {
        LOG(DEBUG, "[Endianess] Conversion needed: data stored in %s-endian, system is %s-endian.",
            IS_DATA_BIG_ENDIAN ? "big" : "little",
            IS_SYSTEM_BIG_ENDIAN() ? "big" : "little")

        buffer[3] = (size >> 24) & 0xFF;
        buffer[2] = (size >> 16) & 0xFF;
        buffer[1] = (size >> 8) & 0xFF;
        buffer[0] = size & 0xFF;
    } else {
        LOG(DEBUG, "[Endianess] No conversion needed: data and system share endianess.")

        buffer[0] = (size >> 24) & 0xFF;
        buffer[1] = (size >> 16) & 0xFF;
        buffer[2] = (size >> 8) & 0xFF;
        buffer[3] = size & 0xFF;
    }
}

/**
 * @brief Obtiene el tamaño de un archivo.
 *
 * @param file Puntero al archivo abierto en modo binario.
 * @return size_t Tamaño del archivo en bytes, o 0 si ocurre un error.
 */
size_t get_file_size(FILE *file) {
    if (file == NULL) {
        LOG(ERROR, "Invalid file pointer in get_file_size.")
        return 0;
    }
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);  // Reset file pointer to the beginning

    return file_size;
}

/**
 * @brief Obtiene la extensión de un archivo a partir de su nombre.
 *
 * @param file_name Nombre del archivo.
 * @return uint8_t* Cadena duplicada con la extensión del archivo (incluyendo el punto), o NULL si no se encuentra una extensión válida.
 *                 El llamante es responsable de liberar la memoria.
 */
uint8_t* get_file_extension(const char *file_name) {
    if (file_name == NULL) {
        LOG(ERROR, "Invalid filename in get_file_extension.")
        return NULL;
    }

    char *dot = strrchr(file_name, '.');
    if (!dot || dot == file_name) {
        LOG(ERROR, "No valid file extension found in %s.", file_name)
        return NULL;
    }

    size_t ext_length = strlen(dot);
    if (ext_length >= EXTENSION_SIZE) {
        LOG(ERROR, "File extension too long in %s.", file_name)
        return NULL;
    }

    return (uint8_t*) strdup(dot);
}