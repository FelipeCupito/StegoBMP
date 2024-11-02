#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp_image.h"
#include "logger.h"

/**
 * @brief Crea una imagen BMP de prueba con dimensiones específicas.
 *
 * @param width        Ancho de la imagen en píxeles.
 * @param height       Altura de la imagen en píxeles.
 * @param fill_color   Valor con el que se llenarán todos los componentes de color.
 * @return BMPImage*   Puntero a la estructura BMPImage creada, o NULL en caso de error.
 */
BMPImage* create_test_bmp(size_t width, size_t height, unsigned char fill_color) {
    if (width == 0 || height == 0) {
        LOG(ERROR, "Width and height must be greater than 0 in create_test_bmp.")
        return NULL;
    }

    BMPImage *bmp = (BMPImage *)malloc(sizeof(BMPImage));
    if (bmp == NULL) {
        LOG(ERROR, "No se pudo asignar memoria para BMPImage en create_test_bmp.")
        return NULL;
    }

    bmp->width = width;
    bmp->height = height;

    // Calcular el tamaño de cada fila con padding (cada fila debe ser múltiplo de 4 bytes)
    size_t row_size = (width * 3 + 3) & ~3; // Multiplo de 4
    bmp->data_size = row_size * height;

    bmp->data = (unsigned char *)malloc(bmp->data_size);
    if (bmp->data == NULL) {
        LOG(ERROR, "No se pudo asignar memoria para los datos de píxeles en create_test_bmp.")
        free(bmp);
        return NULL;
    }

    // Inicializar los datos de píxeles con el color de llenado
    for (size_t row = 0; row < height; row++) {
        for (size_t col = 0; col < width * 3; col++) {
            bmp->data[row * row_size + col] = fill_color;
        }
        // Padding se deja en 0x00
        for (size_t pad = width * 3; pad < row_size; pad++) {
            bmp->data[row * row_size + pad] = 0x00;
        }
    }

    return bmp;
}

/**
 * @brief Imprime los datos de píxeles de una imagen BMP en formato binario para depuración.
 *
 * @param bmp        Puntero a la estructura BMPImage.
 */
void print_pixel_data(const BMPImage *bmp) {
    if (bmp == NULL || bmp->data == NULL) {
        printf("BMPImage es NULL o no tiene datos.\n");
        return;
    }

    size_t row_size = (bmp->width * 3 + 3) & ~3; // Tamaño de la fila con padding

    for (size_t row = 0; row < bmp->height; row++) {
        printf("Fila %zu: ", row + 1);
        for (size_t col = 0; col < bmp->width * 3; col++) {
            // Imprimir el byte en formato binario
            for (int bit = 7; bit >= 0; bit--) {
                printf("%d", (bmp->data[row * row_size + col] >> bit) & 1);
            }
            printf(" ");
            // separo los pixeles
            if (col % 3 == 2) {
                printf("| ");
            }
        }
        // Imprimir padding
        printf(" Padding: ");
        for (size_t pad = bmp->width * 3; pad < row_size; pad++) {
            for (int bit = 7; bit >= 0; bit--) {
                printf("%d", (bmp->data[row * row_size + pad] >> bit) & 1);
            }
            printf(" - ");
        }
        printf("\n");
    }
}

/**
 * @brief Imprime un buffer en formato hexadecimal para ayudar a la depuración.
 */
void print_buffer(const uint8_t *data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

/**
 * @brief Imprime los datos de píxeles de una imagen BMP para depuración.
 *
 * @param data        Puntero a los datos de píxeles.
 * @param data_size   Tamaño de los datos de píxeles en bytes.
 */
void print_data(const unsigned char *data, size_t data_size) {
    for (size_t i = 0; i < data_size; i++) {
        // Imprimir el byte en formato binario
        for (int bit = 7; bit >= 0; bit--) {
            printf("%d", (data[i] >> bit) & 1);
        }
        printf(" - ");
    }
    printf("\n");
}
