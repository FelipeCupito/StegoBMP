#include "utils.h"
#include "logger.h"


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
void adjust_data_endianness(uint8_t *buffer) {
    if (IS_SYSTEM_BIG_ENDIAN() != IS_DATA_BIG_ENDIAN) {
        LOG(DEBUG, "[Endianess] Conversion needed: data stored in %s-endian, system is %s-endian.",
            IS_DATA_BIG_ENDIAN ? "big" : "little",
            IS_SYSTEM_BIG_ENDIAN() ? "big" : "little")

        // Invertir el orden de los 4 bytes
        uint8_t temp = buffer[0];
        buffer[0] = buffer[3];
        buffer[3] = temp;

        temp = buffer[1];
        buffer[1] = buffer[2];
        buffer[2] = temp;

        LOG(DEBUG, "[Endianess] Data endianess adjusted.")
    } else {
        LOG(DEBUG, "[Endianess] No conversion needed: data and system share endianess.");
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