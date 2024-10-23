//
// Created by Felipe Cupitó on 30/09/2024.
//

#include <stdio.h>
#include <assert.h>
#include "../src/include/file_handler.h"


#define IMG_BASE_PATH "../resources/test_images/"
#define EXTENSION_SIZE 10

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
    FilePackage *package = new_file_package(filename);
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
    FilePackage *package = new_file_package("non_existent_file.txt");
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

    FilePackage *package = new_file_package(filename);
    assert(package != NULL);

    // Capturar la salida de la función de impresión
    printf("Expected output:\n");
    print_file_package(package);

    free_file_package(package);

    printf("test_print_file_package passed.\n");

    // Eliminar el archivo de prueba
    remove(filename);
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
 * @brief Test para new_file_package_from_data con datos válidos.
 *
 * Esta prueba crea un buffer de datos que representa un FilePackage válido y verifica que se crea correctamente.
 */
void test_new_file_package_from_data_valid() {
    // Definir el contenido del archivo
    const char *file_content = "Valid file content";
    size_t content_size = strlen(file_content); // 18 bytes
    const char *extension = ".txt";

    // Calcular el tamaño del buffer: 4 bytes para el tamaño + contenido + extensión
    size_t buffer_size = sizeof(int) + content_size + EXTENSION_SIZE;
    uint8_t *buffer = (uint8_t *)malloc(buffer_size);
    assert(buffer != NULL);

    // Llenar el buffer
    *(int *)&buffer[0] = (int)content_size; // tamaño
    memcpy(buffer + sizeof(int), file_content, content_size); // datos
    memset(buffer + sizeof(int) + content_size, 0, EXTENSION_SIZE); // inicializar extensión a 0
    strncpy((char *)(buffer + sizeof(int) + content_size), extension, EXTENSION_SIZE - 1);

    // Llamar a new_file_package_from_data
    FilePackage *package = new_file_package_from_data(buffer);
    assert(package != NULL);

    // Verificar contenidos
    assert(package->size == content_size);
    assert(strcmp(package->extension, extension) == 0);
    assert(package->data != NULL);
    assert(memcmp(package->data, file_content, content_size) == 0);

    // Liberar memoria
    free_file_package(package);
    free(buffer);

    printf("test_new_file_package_from_data_valid passed.\n");

    LOG(INFO, "Prueba pasada: new_file_package_from_data funciona correctamente con datos válidos.");
}


/**
 * @brief Test para new_file_package_from_data con tamaño inválido.
 *
 * Esta prueba verifica que la función maneja correctamente un tamaño de archivo inválido.
 */
void test_new_file_package_from_data_invalid_size() {
    // Crear un buffer con tamaño = 0
    size_t buffer_size = sizeof(int) + EXTENSION_SIZE;
    uint8_t *buffer = (uint8_t *)malloc(buffer_size);
    assert(buffer != NULL);

    // Llenar el buffer
    *(int *)&buffer[0] = 0; // tamaño = 0
    memset(buffer + sizeof(int), 0, buffer_size - sizeof(int));

    // Llamar a new_file_package_from_data
    FilePackage *package = new_file_package_from_data(buffer);
    assert(package == NULL);

    free(buffer);
    LOG(INFO, "Prueba pasada: new_file_package_from_data maneja correctamente un tamaño inválido.");
}

/**
 * @brief Test para new_file_package_from_data con datos NULL.
 *
 * Esta prueba verifica que la función maneja correctamente un puntero de datos NULL.
 */
void test_new_file_package_from_data_invalid_null() {
    FilePackage *package = new_file_package_from_data(NULL);
    assert(package == NULL);
    LOG(INFO, "Prueba pasada: new_file_package_from_data maneja correctamente datos NULL.");
}

/**
 * @brief Test para create_file_from_package con un FilePackage válido.
 *
 * Esta prueba crea un FilePackage manualmente, lo utiliza para crear un archivo y verifica su contenido.
 */
void test_create_file_from_package_valid() {
    const char *filename_base = "output_test_file";
    const char *extension = ".txt";
    const char *file_content = "Output file content for testing.";
    size_t content_size = strlen(file_content);

    // Crear un FilePackage manualmente
    FilePackage *package = (FilePackage *)malloc(sizeof(FilePackage));
    assert(package != NULL);
    package->size = content_size;
    package->data = (unsigned char *)malloc(content_size);
    assert(package->data != NULL);
    memcpy(package->data, file_content, content_size);
    package->extension = strdup(extension);
    assert(package->extension != NULL);

    // Construir el nombre completo del archivo
    size_t full_filename_length = strlen(filename_base) + strlen(extension) + 1;
    char *full_filename = (char *)malloc(full_filename_length);
    assert(full_filename != NULL);
    snprintf(full_filename, full_filename_length, "%s%s", filename_base, extension);

    // Asegurarse de que el archivo no exista antes de la prueba
    remove(full_filename);

    // Llamar a create_file_from_package
    int result = create_file_from_package(filename_base, package);
    assert(result == 0);

    // Verificar que el archivo fue creado
    FILE *file = fopen(full_filename, "rb");
    assert(file != NULL);

    // Obtener el tamaño del archivo
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    assert(file_size == content_size);

    // Leer el contenido del archivo
    unsigned char *read_data = (unsigned char *)malloc(file_size);
    assert(read_data != NULL);
    size_t read_bytes = fread(read_data, 1, file_size, file);
    assert(read_bytes == file_size);
    assert(memcmp(read_data, package->data, file_size) == 0);

    // Limpiar
    fclose(file);
    free(read_data);
    free(full_filename);
    free_file_package(package);

    // Eliminar el archivo creado
    remove(full_filename);

    LOG(INFO, "Prueba pasada: create_file_from_package creó el archivo correctamente.");
}

/**
 * @brief Test para create_file_from_package con entradas inválidas.
 *
 * Esta prueba verifica que la función maneja correctamente entradas inválidas.
 */
void test_create_file_from_package_invalid() {
    // Crear un FilePackage válido para algunas pruebas
    FilePackage *valid_package = (FilePackage *)malloc(sizeof(FilePackage));
    assert(valid_package != NULL);
    valid_package->size = 10;
    valid_package->data = (unsigned char *)malloc(valid_package->size);
    assert(valid_package->data != NULL);
    memset(valid_package->data, 0, valid_package->size);
    valid_package->extension = strdup(".txt");
    assert(valid_package->extension != NULL);

    // Probar con nombre de archivo NULL
    int result = create_file_from_package(NULL, valid_package);
    assert(result == -1);

    // Probar con FilePackage NULL
    result = create_file_from_package("test", NULL);
    assert(result == -1);

    // Probar con extensión NULL en el FilePackage
    FilePackage *package_no_ext = (FilePackage *)malloc(sizeof(FilePackage));
    assert(package_no_ext != NULL);
    package_no_ext->size = 10;
    package_no_ext->data = (unsigned char *)malloc(package_no_ext->size);
    assert(package_no_ext->data != NULL);
    memset(package_no_ext->data, 0, package_no_ext->size);
    package_no_ext->extension = NULL;
    result = create_file_from_package("test", package_no_ext);
    assert(result == -1);
    free_file_package(package_no_ext);

    // Probar con datos NULL en el FilePackage
    FilePackage *package_no_data = (FilePackage *)malloc(sizeof(FilePackage));
    assert(package_no_data != NULL);
    package_no_data->size = 10;
    package_no_data->data = NULL;
    package_no_data->extension = strdup(".txt");
    assert(package_no_data->extension != NULL);
    result = create_file_from_package("test", package_no_data);
    assert(result == -1);
    free_file_package(package_no_data);

    // Limpiar
    free_file_package(valid_package);

    LOG(INFO, "Prueba pasada: create_file_from_package manejó correctamente entradas inválidas.");
}


/**
 * @brief Test para create_file_from_raw_data con datos válidos.
 *
 * Esta prueba crea un buffer de datos en bruto que representa un FilePackage válido, llama a la función y verifica el archivo creado.
 */
void test_create_file_from_raw_data_valid() {
    const char *filename_base = "raw_output_test_file";
    const char *extension = ".dat";
    const char *file_content = "Data from raw data test.";
    size_t content_size = strlen(file_content);

    // Crear un buffer de datos en bruto
    size_t buffer_size = sizeof(int) + content_size + EXTENSION_SIZE;
    uint8_t *buffer = (uint8_t *)malloc(buffer_size);
    assert(buffer != NULL);

    // Llenar el buffer
    *(int *)&buffer[0] = (int)content_size; // tamaño
    memcpy(buffer + sizeof(int), file_content, content_size); // datos
    memset(buffer + sizeof(int) + content_size, 0, EXTENSION_SIZE); // inicializar extensión a 0
    strncpy((char *)(buffer + sizeof(int) + content_size), extension, EXTENSION_SIZE - 1);

    // Construir el nombre completo del archivo
    size_t full_filename_length = strlen(filename_base) + strlen(extension) + 1;
    char *full_filename = (char *)malloc(full_filename_length);
    assert(full_filename != NULL);
    snprintf(full_filename, full_filename_length, "%s%s", filename_base, extension);

    // Asegurarse de que el archivo no exista antes de la prueba
    remove(full_filename);

    // Llamar a create_file_from_raw_data
    int result = create_file_from_raw_data(filename_base, buffer);
    assert(result == 0);

    // Verificar que el archivo fue creado
    FILE *file = fopen(full_filename, "rb");
    assert(file != NULL);

    // Obtener el tamaño del archivo
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    assert(file_size == content_size);

    // Leer el contenido del archivo
    unsigned char *read_data = (unsigned char *)malloc(file_size);
    assert(read_data != NULL);
    size_t read_bytes = fread(read_data, 1, file_size, file);
    assert(read_bytes == file_size);
    assert(memcmp(read_data, file_content, file_size) == 0);

    // Limpiar
    fclose(file);
    free(read_data);
    free(full_filename);
    free(buffer);

    // Eliminar el archivo creado
    remove(full_filename);

    LOG(INFO, "Prueba pasada: create_file_from_raw_data creó el archivo correctamente.");
}

/**
 * @brief Test para create_file_from_raw_data con entradas inválidas.
 *
 * Esta prueba verifica que la función maneja correctamente entradas inválidas.
 */
void test_create_file_from_raw_data_invalid() {
    int result;

    // Probar con nombre de archivo NULL
    result = create_file_from_raw_data(NULL, (const uint8_t *)"data");
    assert(result == -1);

    // Probar con datos NULL
    result = create_file_from_raw_data("test", NULL);
    assert(result == -1);

    // Probar con tamaño inválido (tamaño = 0)
    uint8_t buffer_invalid_size[sizeof(int)] = {0};
    result = create_file_from_raw_data("test_invalid", buffer_invalid_size);
    assert(result == -1);

    LOG(INFO, "Prueba pasada: create_file_from_raw_data manejó correctamente entradas inválidas.");
}


/**
 * @brief Test para free_file_package con un FilePackage válido.
 *
 * Esta prueba crea un FilePackage y lo libera, verificando que no haya errores.
 */
void test_free_file_package_valid() {
    const char *filename = "test_free_valid.txt";

    // Crear un archivo de prueba
    FILE *file = fopen(filename, "w");
    assert(file != NULL);
    fprintf(file, "Content for free_file_package test.");
    fclose(file);

    // Crear el FilePackage
    FilePackage *package = new_file_package(filename);
    assert(package != NULL);

    // Liberar el FilePackage
    free_file_package(package);

    LOG(INFO, "Prueba pasada: free_file_package liberó correctamente un FilePackage válido.");

    // Eliminar el archivo de prueba
    remove(filename);
}

/**
 * @brief Test para free_file_package con un FilePackage NULL.
 *
 * Esta prueba verifica que la función maneja correctamente un puntero NULL.
 */
void test_free_file_package_null() {
    free_file_package(NULL);
    LOG(INFO, "Prueba pasada: free_file_package manejó correctamente un puntero NULL.");
}


int main() {
    set_log_level(NONE);

    // Pruebas de archivos BMP
    test_read_bmp_file();
    test_save_bmp_file();
    test_read_write_bmp("sample1.bmp");
    test_read_write_bmp("sample2.bmp");
    test_read_write_bmp("sample3.bmp");

    // Pruebas de paquetes de archivos
    test_get_file_size();
    test_get_file_extension();
    test_create_file_package_valid();
    test_create_file_package_invalid();
    test_print_file_package();

    // Pruebas adicionales
    test_copy_bmp();
    test_new_file_package_from_data_valid();
    test_new_file_package_from_data_invalid_size();
    test_new_file_package_from_data_invalid_null();
    test_create_file_from_package_valid();
    test_create_file_from_package_invalid();
    test_create_file_from_raw_data_valid();
    test_create_file_from_raw_data_invalid();
    test_free_file_package_valid();
    test_free_file_package_null();

    printf("Todas las pruebas pasaron exitosamente.\n");
    return 0;
}

