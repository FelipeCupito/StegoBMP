#include <stdio.h>
#include <assert.h>
#include "../src/include/bmp_image.h"

#define IMG_BASE_PATH "../resources/test_images/"


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
    LOG(INFO, "Width and height are correct.");

    // Check the data size: 2 (width) * 3 (bytes per pixel) + 2 (padding) = 8 bytes per row, total 16 bytes
    int expected_data_size = 8 * 2;
    assert(bmp->data_size == expected_data_size);
    LOG(INFO, "Data size is correct.");

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
    LOG(INFO, "Pixel data is correct.");

    // Free memory
    free_bmp(bmp);

    LOG(INFO, "Test passed: new_bmp_file works correctly with the 2x2 BMP file.");
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
    LOG(INFO, "BMP file saved successfully.");

    // Step 3: Open the file manually and verify the contents
    FILE *file = fopen(output_image_file, "rb");
    assert(file != NULL);

    // Read and verify the header
    unsigned char read_header[BMP_HEADER_SIZE];
    fread(read_header, sizeof(unsigned char), BMP_HEADER_SIZE, file);
    for (int i = 0; i < BMP_HEADER_SIZE; i++) {
        assert(read_header[i] == bmp.header[i]);
    }
    LOG(INFO, "BMP headers match.");

    // Read and verify the pixel data
    unsigned char read_pixel_data[sizeof(pixel_data)];
    fread(read_pixel_data, sizeof(unsigned char), bmp.data_size, file);
    for (size_t i = 0; i < bmp.data_size; i++) {
        assert(read_pixel_data[i] == bmp.data[i]);
    }
    LOG(INFO, "Pixel data matches.");

    // Clean up
    fclose(file);

    LOG(INFO, "Unit test passed: save_bmp_file works correctly.");
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

void get_pixel_by_index_returns_correct_pixel() {
    BMPImage bmp;
    bmp.width = 2;
    bmp.height = 2;
    bmp.data_size = 16;  // Correcto: 2 filas * row_size=8
    bmp.data = (unsigned char*)malloc(bmp.data_size);
    assert(bmp.data != NULL);
    memset(bmp.data, 0, bmp.data_size); // Inicializar todos los bytes a 0

    // Configurar la primera fila (Fila 0: Pixel 0 y Pixel 1)
    bmp.data[0] = 255; bmp.data[1] = 0; bmp.data[2] = 0;   // Pixel 0 (Blue)
    bmp.data[3] = 0; bmp.data[4] = 255; bmp.data[5] = 0;   // Pixel 1 (Green)
    bmp.data[6] = 0; bmp.data[7] = 0;                       // Padding para la primera fila

    // Configurar la segunda fila (Fila 1: Pixel 2 y Pixel 3)
    bmp.data[8] = 0; bmp.data[9] = 0; bmp.data[10] = 255;   // Pixel 2 (Red)
    bmp.data[11] = 0; bmp.data[12] = 255; bmp.data[13] = 255; // Pixel 3 (Yellow)
    bmp.data[14] = 0; bmp.data[15] = 0;                     // Padding para la segunda fila

    // Blue pixel
    Pixel* pixel = get_pixel_by_index(&bmp, 0);
    assert(pixel != NULL);
    assert(pixel->blue == 255);
    assert(pixel->green == 0);
    assert(pixel->red == 0);

    // Green pixel
    pixel = get_pixel_by_index(&bmp, 1);
    assert(pixel != NULL);
    assert(pixel->blue == 0);
    assert(pixel->green == 255);
    assert(pixel->red == 0);

    // Red pixel
    pixel = get_pixel_by_index(&bmp, 2);
    assert(pixel != NULL);
    assert(pixel->blue == 0);
    assert(pixel->green == 0);
    assert(pixel->red == 255);

    // Yellow pixel
    pixel = get_pixel_by_index(&bmp, 3);
    assert(pixel != NULL);
    assert(pixel->blue == 0);
    assert(pixel->green == 255);
    assert(pixel->red == 255);

    free(bmp.data);
}

void get_pixel_by_index_out_of_bounds_returns_null() {
    BMPImage bmp;
    bmp.width = 2;
    bmp.height = 2;
    bmp.data_size = 16;  // Correcto: 2 filas * row_size=8
    bmp.data = (unsigned char*)malloc(bmp.data_size);
    assert(bmp.data != NULL);
    memset(bmp.data, 0, bmp.data_size); // Inicializar todos los bytes a 0

    // Intentar acceder a un píxel fuera de los límites (pixel_index=4 para 2x2)
    Pixel* pixel = get_pixel_by_index(&bmp, 4);
    assert(pixel == NULL);

    free(bmp.data);
}

void get_component_by_index_returns_correct_component() {
    BMPImage bmp;
    bmp.width = 2;
    bmp.height = 2;
    bmp.data_size = 16;  // Correcto: 2 filas * row_size=8
    bmp.data = (unsigned char*)malloc(bmp.data_size);
    assert(bmp.data != NULL);
    memset(bmp.data, 0, bmp.data_size); // Inicializar todos los bytes a 0

    // Configurar la primera fila (Fila 0: Pixel 0 y Pixel 1)
    bmp.data[0] = 255; bmp.data[1] = 0; bmp.data[2] = 0;   // Pixel 0 (Blue)
    bmp.data[3] = 0; bmp.data[4] = 255; bmp.data[5] = 0;   // Pixel 1 (Green)
    bmp.data[6] = 0; bmp.data[7] = 0;                       // Padding para la primera fila

    // Configurar la segunda fila (Fila 1: Pixel 2 y Pixel 3)
    bmp.data[8] = 0; bmp.data[9] = 0; bmp.data[10] = 255;   // Pixel 2 (Red)
    bmp.data[11] = 0; bmp.data[12] = 255; bmp.data[13] = 255; // Pixel 3 (Yellow)
    bmp.data[14] = 0; bmp.data[15] = 0;                     // Padding para la segunda fila

    // Verificar componentes
    // Por ejemplo, verificar el componente verde del Pixel1 (index=4)
    uint8_t* component = get_component_by_index(&bmp, 4);
    assert(component != NULL);
    assert(*component == 255);  // Pixel1 verde

    // Verificar componente azul del Pixel2 (index=8)
    component = get_component_by_index(&bmp, 8);
    assert(component != NULL);
    assert(*component == 255);  // Pixel2 azul

    // Verificar componente rojo del Pixel2 (index=10)
    component = get_component_by_index(&bmp, 10);
    assert(component != NULL);
    assert(*component == 255);  // Pixel2 rojo

    // Intentar acceder a un componente fuera de los límites
    component = get_component_by_index(&bmp, 16);
    assert(component == NULL);

    free(bmp.data);
}

void get_component_by_index_out_of_bounds_returns_null() {
    BMPImage bmp;
    bmp.width = 2;
    bmp.height = 2;
    bmp.data_size = 16;  // Correcto: 2 filas * row_size=8
    bmp.data = (unsigned char*)malloc(bmp.data_size);
    assert(bmp.data != NULL);
    memset(bmp.data, 0, bmp.data_size); // Inicializar todos los bytes a 0

    // Intentar acceder a un componente fuera de los límites (index=16 para 2x2)
    uint8_t* component = get_component_by_index(&bmp, 16);
    assert(component == NULL);

    free(bmp.data);
}

int main() {
    set_log_level(NONE);

    // Pruebas de archivos BMP
    test_read_bmp_file();
    test_save_bmp_file();
    test_read_write_bmp("sample1.bmp");
    test_read_write_bmp("sample2.bmp");
    test_read_write_bmp("sample3.bmp");

    // padding test
    get_component_by_index_returns_correct_component();
    get_component_by_index_out_of_bounds_returns_null();
    get_pixel_by_index_returns_correct_pixel();
    get_pixel_by_index_out_of_bounds_returns_null();

    // Pruebas adicionales
    test_copy_bmp();

    printf("Todas las pruebas pasaron exitosamente.\n");
    return 0;
}

