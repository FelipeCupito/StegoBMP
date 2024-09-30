//
// Created by Felipe Cupitó on 30/09/2024.
//
#ifndef STEGOBMP_FILE_HANDLER_H
#define STEGOBMP_FILE_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"

// Structure to hold file data and metadata
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
FilePackage *create_file_package(const char *filepath);

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

#ifdef TESTING
/**
 * Only used for testing purposes.
 */
size_t get_file_size(FILE *file);
char *get_file_extension(const char *filename);
#endif

#endif //STEGOBMP_FILE_HANDLER_H
