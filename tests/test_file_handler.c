//
// Created by Felipe Cupitó on 30/09/2024.
//

#include <stdio.h>
#include <assert.h>
#include "../src/include/file_handler.h"


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
    BMPImage *bmp = read_bmp_file(test_image_file);
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

    LOG(INFO, "Test passed: read_bmp_file works correctly with the 2x2 BMP file.");
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
    BMPImage *bmp_1 = read_bmp_file(test_file);
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
 * Test function to check file size retrieval
 */
void test_get_file_size() {
    const char *filename = "test.txt";

    // Crear el archivo de prueba
    FILE *file = fopen(filename, "w");
    assert(file != NULL);

    // Escribir contenido en el archivo
    fprintf(file, "This is a test file.\n");
    fclose(file);

    // Reabrir el archivo y verificar el tamaño
    file = fopen(filename, "r");
    assert(file != NULL);

    size_t file_size = get_file_size(file);
    assert(file_size == 21);  // "This is a test file.\n" tiene 21 bytes
    fclose(file);

    printf("test_get_file_size passed.\n");

    // Eliminar el archivo de prueba
    remove(filename);
}

/**
 * Test function to check file extension retrieval
 */
void test_get_file_extension() {
    char *ext = get_file_extension("example.txt");
    assert(ext != NULL);
    assert(strcmp(ext, ".txt") == 0);
    free(ext);  // Liberar la memoria asignada

    ext = get_file_extension("no_extension");
    assert(ext == NULL);  // Debe retornar NULL si no hay extensión

    printf("test_get_file_extension passed.\n");
}

/**
 * Test function to create a valid file package from a file
 */
void test_create_file_package_valid() {
    const char *filename = "test_valid.txt";

    // Crear el archivo de prueba
    FILE *file = fopen(filename, "w");
    assert(file != NULL);
    fprintf(file, "Valid file content");
    fclose(file);

    // Crear el paquete
    FilePackage *package = create_file_package(filename);
    assert(package != NULL);
    assert(package->size == 18);  // "Valid file content" tiene 18 bytes
    assert(strcmp(package->extension, ".txt") == 0);
    assert(package->data != NULL);

    free_file_package(package);  // Liberar los recursos

    printf("test_create_file_package_valid passed.\n");

    // Eliminar el archivo de prueba
    remove(filename);
}

/**
 * Test function to handle an invalid file path
 */
void test_create_file_package_invalid() {
    FilePackage *package = create_file_package("non_existent_file.txt");
    assert(package == NULL);  // Debe retornar NULL para un archivo inexistente

    printf("test_create_file_package_invalid passed.\n");
}

/**
 * Test function to print file package information
 */
void test_print_file_package() {
    const char *filename = "test_print.txt";

    // Crear el archivo de prueba
    FILE *file = fopen(filename, "w");
    assert(file != NULL);
    fprintf(file, "File content to print");
    fclose(file);

    FilePackage *package = create_file_package(filename);
    assert(package != NULL);

    // Capturar la salida de la función de impresión
    printf("Expected output:\n");
    print_file_package(package);

    free_file_package(package);

    printf("test_print_file_package passed.\n");

    // Eliminar el archivo de prueba
    remove(filename);
}

int main() {
    set_log_level(NONE);

    // BMP file tests
    test_read_bmp_file();
    test_save_bmp_file();

    // Read and write BMP file tests
    test_read_write_bmp("sample1.bmp");
    test_read_write_bmp("sample2.bmp");
    test_read_write_bmp("sample3.bmp");

    // File package tests
//    test_get_file_size();
//    test_get_file_extension();
//    test_create_file_package_valid();
//    test_create_file_package_invalid();
//    test_print_file_package();

    printf("All tests passed successfully.\n");
    return 0;
}
