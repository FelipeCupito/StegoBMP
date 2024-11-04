#ifndef STEGOBMP_BMP_IMAGE_H
#define STEGOBMP_BMP_IMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "utils.h"

#define BMP_HEADER_SIZE 54  // Total size of the BMP header for V3 format

/**
 * @brief Structure to hold BMP image data, including header and pixel data.
 */
typedef struct {
    unsigned char header[BMP_HEADER_SIZE];   // BMP header (54 bytes for V3 format)
    unsigned char *data;                     // Pointer to the pixel data
    size_t data_size;                        // Size of the pixel data in bytes and padding
    size_t width;                            // Width of the image in pixels
    size_t height;                           // Height of the image in pixels
} BMPImage;

typedef enum {
    BLUE = 0,
    GREEN = 1,
    RED = 2,
    INVALID_COLOR
} ColorType;

typedef struct {
    uint8_t *component_ptr; // Puntero al componente de color
    ColorType color;        // Tipo de color (BLUE, GREEN, RED)
} Component;

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
 * @param bmp Pointer to the BMPImage structure to free.
 */
void free_bmp(BMPImage *bmp);

/**
 * @brief Creates a deep copy of a BMPImage structure.
 *
 * @param bmp Pointer to the BMPImage structure to copy.
 * @return BMPImage* Pointer to a new BMPImage structure with copied data.
 */
BMPImage* copy_bmp(BMPImage *bmp);

/**
 * @brief Obtiene un puntero al componente de color (byte) en un BMP según el índice de componente global.
 *
 * @param bmp        Puntero a la estructura BMPImage.
 * @param index      Índice del componente de color (byte) a acceder.
 * @return Component Puntero al componente y su color correspondiente (modificable).
 */
Component get_component_by_index(const BMPImage *bmp, size_t index);


#endif //STEGOBMP_BMP_IMAGE_H
