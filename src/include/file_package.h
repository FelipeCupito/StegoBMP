#ifndef STEGOBMP_FILE_PACKAGE_H
#define STEGOBMP_FILE_PACKAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "logger.h"

/**
 * @brief Structure to hold message file data in memory.
 */
typedef struct {
    size_t size;            // Size of the file
    unsigned char *data;    // File data (bitmap)
    char *extension;        // File extension (e.g., .txt, .bmp)
} FilePackage;


/**
 * Create a FilePackage by reading the file data into memory.
 * @param file_path Path to the file
 * @return Pointer to a dynamically allocated FilePackage structure, or NULL on error.
 *         The caller must free the FilePackage using free_file_package.
 */
FilePackage *new_file_package(const char *filepath);

/**
 * @brief Creates a FilePackage from raw data in memory.
 *
 * This function interprets the provided raw data to extract the file size, data, and extension,
 * and stores them in a dynamically allocated FilePackage structure.
 *
 * @param data Pointer to the raw data buffer.
 * @return FilePackage* Pointer to a dynamically allocated FilePackage structure, or NULL on error.
 *         The caller must free the FilePackage using `free_file_package`.
 */
FilePackage *new_file_package_from_data(const uint8_t *data);

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
 * @param filename The name of the file without the extension.
 * @param package Pointer to the FilePackage that contains the data and extension.
 * @return 0 if the file was created successfully, -1 on error.
 */
int create_file_from_package(const char *filename, FilePackage *package);

/**
 *
 * */
int create_file_from_raw_data(const char *filename, const uint8_t *data);


#ifdef TESTING
/**
 * Only used for testing purposes.
 */
size_t get_file_size(FILE *file);
char *get_file_extension(const char *filename);
#endif


#endif //STEGOBMP_FILE_PACKAGE_H
