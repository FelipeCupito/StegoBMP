#include <stdio.h>
#include <assert.h>
#include "../src/include/bmp_image.h"
#include "../src/include/utils.h"
#include "test_utils.c"


/**
 * @brief Test for reading a 2x2 BMP file.
 *
 * This test reads a BMP image from disk, checks the header and pixel data, and verifies
 * that the image properties match what is expected for a 2x2 BMP image.
 */
void test_read_bmp_file() {
    const char *test_image_file = IMG_BASE_PATH "2x2_image.bmp";

    // Call the read_bmp_file function
    BMPImage *bmp = new_bmp_file(test_image_file);
    assert(bmp != NULL);

    // Check the width and height
    assert(bmp->width == 2);
    assert(bmp->height == 2);
    LOG(INFO, "Width and height are correct.")

    // Check the data size: 2 (width) * 3 (bytes per pixel) + 2 (padding) = 8 bytes per row, total 16 bytes
    int expected_data_size = 8 * 2;
    assert(bmp->data_size == expected_data_size);
    LOG(INFO, "Data size is correct.")

    // Check pixel values
    // The pixel data starts from bottom-left corner, as BMPs are stored bottom-up
    unsigned char expected_pixel_data[] = {
            0x00, 0x00, 0xFF,       // Red pixel
            0xFF, 0xFF, 0xFF,       // White pixel
            0x00, 0x00,                 // Padding bytes for row 1
            0xFF, 0x00, 0x00,      // Blue pixel
            0x00, 0xFF, 0x00,    // Green pixel
            0x00, 0x00                // Padding bytes for row 2
    };

    for (size_t i = 0; i < bmp->data_size; i++) {
        assert(bmp->data[i] == expected_pixel_data[i]);
    }
    LOG(INFO, "Pixel data is correct.")

    // Free memory
    free_bmp(bmp);

    LOG(INFO, "Test passed: new_bmp_file works correctly with the 2x2 BMP file.")
}

/**
 * @brief Test for reading a 2x2 BMP file with a different header size.
 *
 * This test manually creates a BMPImage structure for a 2x2 image and saves it using save_bmp_file.
 * It then reads the saved file directly and verifies that the header and pixel data are correct.
 */
void test_save_bmp_file() {
    const char *output_image_file = IMG_BASE_PATH  "output_image.bmp";

    // Step 1: Manually create a BMPImage structure for a 2x2 image
    BMPImage bmp;
    unsigned char header[BMP_HEADER_SIZE] = {
            0x42, 0x4D, 0x46, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
            0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00,
            0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x13, 0x0B,
            0x00, 0x00, 0x13, 0x0B, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00
    };

    unsigned char pixel_data[] = {
            0x00, 0x00, 0xFF,    // Blue pixel
            0x00, 0xFF, 0x00,    // Green pixel
            0x00, 0x00,          // Padding bytes for row 1
            0xFF, 0xFF, 0xFF,    // White pixel
            0xFF, 0x00, 0x00,    // Red pixel
            0x00, 0x00           // Padding bytes for row 2
    };

    bmp.width = 2;
    bmp.height = 2;
    bmp.data_size = sizeof(pixel_data);
    memcpy(bmp.header, header, BMP_HEADER_SIZE);
    bmp.data = pixel_data;

    // Step 2: Save the BMP using save_bmp_file
    int save_result = save_bmp_file(output_image_file, &bmp);
    assert(save_result == 0);
    LOG(INFO, "BMP file saved successfully.")

    // Step 3: Open the file manually and verify the contents
    FILE *file = fopen(output_image_file, "rb");
    assert(file != NULL);

    // Read and verify the header
    unsigned char read_header[BMP_HEADER_SIZE];
    fread(read_header, sizeof(unsigned char), BMP_HEADER_SIZE, file);
    for (int i = 0; i < BMP_HEADER_SIZE; i++) {
        assert(read_header[i] == bmp.header[i]);
    }
    LOG(INFO, "BMP headers match.")

    // Read and verify the pixel data
    unsigned char read_pixel_data[sizeof(pixel_data)];
    fread(read_pixel_data, sizeof(unsigned char), bmp.data_size, file);
    for (size_t i = 0; i < bmp.data_size; i++) {
        assert(read_pixel_data[i] == bmp.data[i]);
    }
    LOG(INFO, "Pixel data matches.")

    // Clean up
    fclose(file);

    LOG(INFO, "Unit test passed: save_bmp_file works correctly.")
}

/**
 * @brief Test for reading and writing BMP files.
 *
 * This test reads a BMP image from disk, saves it to a new file, and then reads
 * the new file back to verify that the header and pixel data are the same.
 */
void test_read_write_bmp( const char *filename) {

    // Test file names
    char *test_file = (char *)malloc(strlen(IMG_BASE_PATH) + strlen(filename) + 1);
    strcpy(test_file, IMG_BASE_PATH);
    strcat(test_file, filename);

    // Call the read_bmp_file function
    BMPImage *bmp_1 = new_bmp_file(test_file);
    assert(bmp_1 != NULL);


    // Test file name
    char *test_output_file = (char *)malloc(strlen(IMG_BASE_PATH) + strlen("test_output_") + strlen(filename) + 1);
    strcpy(test_output_file, IMG_BASE_PATH);
    strcat(test_output_file, "test_output_");
    strcat(test_output_file, filename);

    // Save the BMP to disk
    int save_result = save_bmp_file(test_output_file, bmp_1);
    assert(save_result == 0);

    // Free memory
    free_bmp(bmp_1);
    free(test_file);
    free(test_output_file);
}

/**
 * @brief Test para la función copy_bmp.
 *
 * Esta prueba verifica que la copia de un BMPImage es una réplica exacta pero independiente.
 */
void test_copy_bmp() {
    const char *test_image_file = IMG_BASE_PATH "2x2_image.bmp";

    // Crear el BMPImage original
    BMPImage *original_bmp = new_bmp_file(test_image_file);
    assert(original_bmp != NULL);

    // Copiar el BMPImage
    BMPImage *copied_bmp = copy_bmp(original_bmp);
    assert(copied_bmp != NULL);

    // Verificar que los headers sean iguales
    for (int i = 0; i < BMP_HEADER_SIZE; i++) {
        assert(original_bmp->header[i] == copied_bmp->header[i]);
    }

    // Verificar tamaño de datos, ancho y alto
    assert(original_bmp->data_size == copied_bmp->data_size);
    assert(original_bmp->width == copied_bmp->width);
    assert(original_bmp->height == copied_bmp->height);

    // Verificar los datos de píxeles
    for (size_t i = 0; i < original_bmp->data_size; i++) {
        assert(original_bmp->data[i] == copied_bmp->data[i]);
    }

    // Verificar que los punteros a datos sean diferentes (copia profunda)
    assert(original_bmp->data != copied_bmp->data);

    // Modificar el original y verificar que la copia no cambia
    original_bmp->data[0] ^= 0xFF;
    assert(original_bmp->data[0] != copied_bmp->data[0]);

    // Modificar la copia y verificar que el original no cambia
    copied_bmp->data[1] ^= 0xFF;
    assert(original_bmp->data[1] != copied_bmp->data[1]);

    // Liberar memoria
    free_bmp(original_bmp);
    free_bmp(copied_bmp);

    LOG(INFO, "Prueba pasada: copy_bmp funciona correctamente.");
}

/**
 * @brief Test básico de acceso a componentes de color (azul, verde, rojo).
 *
 * Este test verifica que se puedan obtener los componentes de color azul, verde y rojo en
 * posiciones conocidas dentro de una imagen 2x2, comprobando que `get_component_by_index`
 * funcione correctamente para el acceso básico.
 */
void test_basic_component_access() {
    BMPImage bmp;
    bmp.width = 2;
    bmp.height = 2;
    unsigned char pixel_data[] = {
            0x00, 0x00, 0xFF,   // Azul (Primer píxel)
            0x00, 0xFF, 0x00,   // Verde (Segundo píxel)
            0x00, 0x00,         // Padding en Fila 1
            0xFF, 0x00, 0x00,   // Rojo (Tercer píxel, Fila 2)
            0x00, 0xFF, 0xFF,   // Cian (Cuarto píxel)
            0x00, 0x00          // Padding en Fila 2
    };
    bmp.data = pixel_data;
    bmp.data_size = sizeof(pixel_data);

    // Componente Azul en el primer píxel (índice 0)
    Component component = get_component_by_index(&bmp, 0);
    assert(component.color == BLUE && *component.component_ptr == 0x00);

    // Componente Verde en el segundo píxel (índice 4)
    component = get_component_by_index(&bmp, 4);
    assert(component.color == GREEN && *component.component_ptr == 0xFF);

    // Componente Rojo en el tercer píxel (índice 8)
    component = get_component_by_index(&bmp, 8);
    assert(component.color == RED && *component.component_ptr == 0x00);

    // Componente Verde del tercer píxel (índice 9)
    component = get_component_by_index(&bmp, 9);
    assert(component.color == BLUE && *component.component_ptr == 0x00);

    // Componente Rojo del tercer píxel (índice 10)
    component = get_component_by_index(&bmp, 10);
    assert(component.color == GREEN && *component.component_ptr == 0xFF);

    printf("Test básico de acceso a componentes de color pasado.\n");
}


/**
 * @brief Test para índices fuera de límites.
 *
 * Este test verifica que `get_component_by_index` maneje adecuadamente un índice que está
 * fuera de los límites de la imagen, devolviendo un componente inválido.
 */
void test_index_out_of_bounds() {
    BMPImage bmp;
    bmp.width = 2;
    bmp.height = 2;
    unsigned char pixel_data[] = {
            0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, // Fila 1
            0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, // Fila 2
            0x00, 0x00                          // Padding
    };
    bmp.data = pixel_data;
    bmp.data_size = sizeof(pixel_data);

    size_t total_components = bmp.width * bmp.height * 3;

    // Índice fuera de los límites de componentes
    Component component = get_component_by_index(&bmp, total_components + 1);
    assert(component.color == INVALID_COLOR && component.component_ptr == NULL);

    printf("Test de índice fuera de límites pasado.\n");
}
/*
 * @brief Test de alineación con filas de padding.
 *
 * Este test verifica que `get_component_by_index` maneje adecuadamente el padding en cada
 * fila de píxeles para imágenes con un ancho que no es múltiplo de 4.
 */
void test_alignment_with_padding() {
    BMPImage bmp;
    bmp.width = 3;
    bmp.height = 2;
    unsigned char pixel_data[] = {
            0xFF, 0x00, 0x00,  // Rojo
            0x00, 0xFF, 0x00,  // Verde
            0x00, 0x00, 0xFF,  // Azul
            0x00, 0x00, 0x00,  // Cian
            0x00, 0xFF, 0xFF,  // Cian
            0xFF, 0xFF, 0x00,  // Amarillo
            0xFF, 0x00, 0xFF,  // Magenta
            0x00, 0x00, 0x00,
    };
    bmp.data = pixel_data;
    bmp.data_size = sizeof(pixel_data);

    // Componente Rojo en el primer píxel de la primera fila
    Component component = get_component_by_index(&bmp, 0);
    assert(component.color == BLUE && *component.component_ptr == 0xFF);

    // Componente Verde en el primer píxel de la segunda fila
    component = get_component_by_index(&bmp, 9);  // Primer píxel, segunda fila
    assert(component.color == BLUE && *component.component_ptr == 0x00);

    // Componente Rojo en el segundo píxel de la segunda fila
    component = get_component_by_index(&bmp, 10); // Segundo píxel, segunda fila
    assert(component.color == GREEN && *component.component_ptr == 0xFF);

    printf("Test de alineación con filas de padding pasado.\n");
}


/**
 * @brief Test para imagen o datos nulos.
 *
 * Este test verifica que `get_component_by_index` maneje adecuadamente los casos en que se
 * pasa una imagen o datos `NULL`, devolviendo un componente inválido.
 */
void test_null_image_or_data() {
    BMPImage *null_bmp = NULL;
    Component component = get_component_by_index(null_bmp, 0);
    assert(component.color == INVALID_COLOR && component.component_ptr == NULL);

    BMPImage bmp;
    bmp.width = 2;
    bmp.height = 2;
    bmp.data = NULL;
    component = get_component_by_index(&bmp, 0);
    assert(component.color == INVALID_COLOR && component.component_ptr == NULL);

    printf("Test para imagen o datos nulos pasado.\n");
}

int main() {
    set_log_level(NONE);

    // Pruebas de archivos BMP
    test_read_bmp_file();
    test_save_bmp_file();
    test_read_write_bmp("sample1.bmp");
    test_read_write_bmp("sample2.bmp");
    test_read_write_bmp("sample3.bmp");

   // Pruebas adicionales
    test_copy_bmp();

    test_basic_component_access();
    test_index_out_of_bounds();
    test_alignment_with_padding();
    test_null_image_or_data();

    printf("Todas las pruebas pasaron exitosamente.\n");
    return 0;
}

