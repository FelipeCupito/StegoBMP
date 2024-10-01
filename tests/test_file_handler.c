//
// Created by Felipe Cupitó on 30/09/2024.
//

#include <stdio.h>
#include <assert.h>
#include "../src/include/file_handler.h"

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

    // Ejecutar las pruebas
    test_get_file_size();
    test_get_file_extension();
    test_create_file_package_valid();
    test_create_file_package_invalid();
    test_print_file_package();

    printf("All tests passed successfully.\n");
    return 0;
}
