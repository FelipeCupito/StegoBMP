#ifndef STEGOBMP_FILE_HANDLER_H
#define STEGOBMP_FILE_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"

#define BMP_HEADER_SIZE 54      // Total size of the BMP header for V3 format

/**
 * @brief Structure to hold BMP image data, including header and pixel data.
 */
typedef struct {
    unsigned char header[BMP_HEADER_SIZE];   // BMP header (54 bytes for V3 format)
    unsigned char *data;                     // Pointer to the pixel data
    size_t data_size;                        // Size of the pixel data in bytes
    size_t width;                            // Width of the image in pixels
    size_t height;                           // Height of the image in pixels
} BMPImage;


/**
 * @brief Structure to hold message file data in memory.
 */
typedef struct {
    size_t size;            // Size of the file
    unsigned char *data;    // File data (bitmap)
    char *extension;        // File extension (e.g., .txt, .bmp)
} FilePackage;


/**
 * @brief Reads a BMP file from disk and loads its header and pixel data.
 *
 * This function checks that the BMP file is in the V3 format (54-byte header),
 * has a depth of 24 bits per pixel, and is not compressed. It then reads the
 * image pixel data into memory.
 *
 * @param file_path Path to the BMP file on disk.
 * @return BMPImage* Pointer to a dynamically allocated BMPImage structure, or NULL if an error occurred.
 */
BMPImage *new_bmp_file(const char *file_path);

/**
 * @brief Saves a BMPImage to a file.
 *
 * This function writes the BMP header and pixel data to the specified output file.
 * It ensures that both the header and the pixel data are properly written to disk.
 *
 * @param output_file Path to the output BMP file.
 * @param bmp Pointer to a BMPImage structure that holds the header and pixel data.
 * @return int Returns 0 if the file is successfully saved, or -1 if an error occurs.
 */
int save_bmp_file(const char *output_file, BMPImage *bmp);

/**
 * @brief Frees the memory associated with a BMPImage.
 *
 * This function frees the dynamically allocated memory for the pixel data
 * and the BMPImage structure itself.
 *
 * @param bmp Pointer to the BMPImage structure to free.
 */
void free_bmp(BMPImage *bmp);


BMPImage* copy_bmp(BMPImage *bmp);
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

#ifdef TESTING
/**
 * Only used for testing purposes.
 */
size_t get_file_size(FILE *file);
char *get_file_extension(const char *filename);
#endif

#endif //STEGOBMP_FILE_HANDLER_H
