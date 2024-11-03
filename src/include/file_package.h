#ifndef STEGOBMP_FILE_PACKAGE_H
#define STEGOBMP_FILE_PACKAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "logger.h"

/**
 * @brief Structure to hold message file data in memory.
 */
typedef struct {
    uint32_t size;          // Size of the file
    uint8_t* data;          // File data (bitmap)
    uint8_t* extension;     // File extension (e.g., .txt, .bmp)
} FilePackage;


/**
 * Create a FilePackage by reading the file data into memory.
 * @param file_path Path to the file
 * @return Pointer to a dynamically allocated FilePackage structure, or NULL on error.
 *         The caller must free the FilePackage using free_file_package.
 */
FilePackage *new_file_package(const char *file_path);

/**
 * @brief Crea un buffer con todos los datos listos para embebed a partir de un archivo.
 *
 * Esta función lee el archivo especificado en el `file_path`, obtiene el tamaño del archivo,
 * sus datos y extensión, y los concatena en un buffer en el formato:
 * `tamaño de datos || datos || extensión`.
 *
 * @param file_path  Ruta al archivo que se va a leer.
 * @param buffer_size Puntero donde se almacenará el tamaño total del buffer creado.
 * @return uint8_t*  Puntero al buffer creado con los datos para embebeder.
 *                   El llamante es responsable de liberar la memoria. Devuelve NULL si ocurre algún error.
 */
uint8_t* embed_data_from_file(const char *file_path, size_t *buffer_size);


///**
// * @brief Creates a FilePackage from raw data in memory.
// *
// * This function interprets the provided raw data to extract the file size, data, and extension,
// * and stores them in a dynamically allocated FilePackage structure.
// *
// * @param data Pointer to the raw data buffer.
// * @return FilePackage* Pointer to a dynamically allocated FilePackage structure, or NULL on error.
// *         The caller must free the FilePackage using `free_file_package`.
// */
//FilePackage *new_file_package_from_data(const uint8_t *data);



///**
// * @brief Crea un buffer que contiene el tamaño de los datos, los datos y la extensión concatenados.
// *
// * @param package      Puntero al FilePackage que contiene los datos y la extensión.
// * @param buffer_size  Puntero donde se almacenará el tamaño total del buffer creado.
// * @return uint8_t*    Puntero al buffer creado. El llamante es responsable de liberar la memoria.
// */
//uint8_t* create_data_buffer(const FilePackage *package, size_t *buffer_size);


/**
 * Free the memory associated with a FilePackage.
 * @param package Pointer to the FilePackage
 */
void free_file_package(FilePackage *package);

/**
 * Print the information contained in a FilePackage.
 * @param package Pointer to the FilePackage
 */
void print_file_package(FilePackage *package);

/**
 * Create a file from the given FilePackage.
 * The filename will be constructed by concatenating the provided filename and the extension in the FilePackage.
 *
 * @param file_name The name of the file without the extension.
 * @param package Pointer to the FilePackage that contains the data and extension.
 * @return 0 if the file was created successfully, -1 on error.
 */
int create_file_from_package(const char *file_name, FilePackage *package);

/**
 *
 * */
int create_file_from_raw_data(const char *file_name, const uint8_t *data);


#ifdef TESTING
/**
 * Only used for testing purposes.
 */
size_t get_file_size(FILE *file);
uint8_t* get_file_extension(const char *file_name);
#endif


#endif //STEGOBMP_FILE_PACKAGE_H
