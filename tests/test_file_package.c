#include <stdio.h>
#include <assert.h>
#include "../src/include/file_package.h"

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
    uint8_t *ext = get_file_extension("example.txt");
    assert(ext != NULL);
    assert(strcmp((char*)ext, ".txt") == 0);
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
    assert(strcmp((char*)package->extension, ".txt") == 0);
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

///**
// * @brief Test para new_file_package_from_data con datos válidos.
// *
// * Esta prueba crea un buffer de datos que representa un FilePackage válido y verifica que se crea correctamente.
// */
//void test_new_file_package_from_data_valid() {
//    // Definir el contenido del archivo
//    const char *file_content = "Valid file content";
//    size_t content_size = strlen(file_content); // 18 bytes
//    const char *extension = ".txt";
//
//    // Calcular el tamaño del buffer: 4 bytes para el tamaño + contenido + extensión
//    size_t buffer_size = sizeof(int) + content_size + EXTENSION_SIZE;
//    uint8_t *buffer = (uint8_t *)malloc(buffer_size);
//    assert(buffer != NULL);
//
//    // Llenar el buffer
//    *(int *)&buffer[0] = (int)content_size; // tamaño
//    memcpy(buffer + sizeof(int), file_content, content_size); // datos
//    memset(buffer + sizeof(int) + content_size, 0, EXTENSION_SIZE); // inicializar extensión a 0
//    strncpy((char *)(buffer + sizeof(int) + content_size), extension, EXTENSION_SIZE - 1);
//
//    // Llamar a new_file_package_from_data
//    FilePackage *package = new_file_package_from_data(buffer);
//    assert(package != NULL);
//
//    // Verificar contenidos
//    assert(package->size == content_size);
//    assert(strcmp(package->extension, extension) == 0);
//    assert(package->data != NULL);
//    assert(memcmp(package->data, file_content, content_size) == 0);
//
//    // Liberar memoria
//    free_file_package(package);
//    free(buffer);
//
//    printf("test_new_file_package_from_data_valid passed.\n");
//
//    LOG(INFO, "Prueba pasada: new_file_package_from_data funciona correctamente con datos válidos.");
//}


///**
// * @brief Test para new_file_package_from_data con tamaño inválido.
// *
// * Esta prueba verifica que la función maneja correctamente un tamaño de archivo inválido.
// */
//void test_new_file_package_from_data_invalid_size() {
//    // Crear un buffer con tamaño = 0
//    size_t buffer_size = sizeof(int) + EXTENSION_SIZE;
//    uint8_t *buffer = (uint8_t *)malloc(buffer_size);
//    assert(buffer != NULL);
//
//    // Llenar el buffer
//    *(int *)&buffer[0] = 0; // tamaño = 0
//    memset(buffer + sizeof(int), 0, buffer_size - sizeof(int));
//
//    // Llamar a new_file_package_from_data
//    FilePackage *package = new_file_package_from_data(buffer);
//    assert(package == NULL);
//
//    free(buffer);
//    LOG(INFO, "Prueba pasada: new_file_package_from_data maneja correctamente un tamaño inválido.");
//}

///**
// * @brief Test para new_file_package_from_data con datos NULL.
// *
// * Esta prueba verifica que la función maneja correctamente un puntero de datos NULL.
// */
//void test_new_file_package_from_data_invalid_null() {
//    FilePackage *package = new_file_package_from_data(NULL);
//    assert(package == NULL);
//    LOG(INFO, "Prueba pasada: new_file_package_from_data maneja correctamente datos NULL.");
//}

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
    package->extension = (uint8_t*) strdup(extension);
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
    assert(result == 1);

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

    LOG(INFO, "Prueba pasada: create_file_from_package creó el archivo correctamente.")
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
    valid_package->extension = (uint8_t*) strdup(".txt");
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
    package_no_data->extension = (uint8_t*) strdup(".txt");
    assert(package_no_data->extension != NULL);
    result = create_file_from_package("test", package_no_data);
    assert(result == -1);
    free_file_package(package_no_data);

    // Limpiar
    free_file_package(valid_package);

    LOG(INFO, "Prueba pasada: create_file_from_package manejó correctamente entradas inválidas.")
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
    assert(result == 1);

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

    LOG(INFO, "Prueba pasada: create_file_from_raw_data creó el archivo correctamente.")
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

    LOG(INFO, "Prueba pasada: create_file_from_raw_data manejó correctamente entradas inválidas.")
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

    LOG(INFO, "Prueba pasada: free_file_package liberó correctamente un FilePackage válido.")

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
    LOG(INFO, "Prueba pasada: free_file_package manejó correctamente un puntero NULL.")
}

/**
 * @brief Test para embed_data_from_file con un archivo válido.
 *
 * Esta prueba crea un archivo de prueba, llama a embed_data_from_file y verifica que el buffer
 * creado contenga el tamaño correcto, los datos del archivo y la extensión adecuada.
 */
void test_embed_data_from_file_valid() {
    const char *filename = "embed_test_valid.txt";
    const char *file_content = "Data to embed into the buffer.";
    const char *extension = ".txt";
    size_t content_size = strlen(file_content);

    // Crear el archivo de prueba
    FILE *file = fopen(filename, "w");
    assert(file != NULL);
    fprintf(file, "%s", file_content);
    fclose(file);

    // Llamar a embed_data_from_file
    size_t buffer_size = 0;
    uint8_t *buffer = embed_data_from_file(filename, &buffer_size);
    assert(buffer != NULL);

    // Verificar el tamaño del buffer: 4 bytes para el tamaño + contenido + EXTENSION_SIZE
    assert(buffer_size == sizeof(uint32_t) + content_size + 5);

    // Extraer y verificar el tamaño del archivo del buffer
    uint32_t  extracted_size = *(uint32_t *)buffer;
    assert(extracted_size == content_size);

    // Verificar los datos del archivo
    assert(memcmp(buffer + sizeof(uint32_t), file_content, content_size) == 0);

    // Verificar la extensión
    char extracted_extension[EXTENSION_SIZE] = {0};
    strncpy(extracted_extension, (char *)(buffer + sizeof(uint32_t) + content_size), EXTENSION_SIZE - 1);
    extracted_extension[EXTENSION_SIZE - 1] = '\0';
    assert(strcmp(extracted_extension, extension) == 0);

    // Limpiar
    free(buffer);
    remove(filename);
}

/**
 * @brief Test para embed_data_from_file con ruta de archivo NULL.
 *
 * Esta prueba verifica que la función retorna NULL cuando se le pasa una ruta de archivo NULL.
 */
void test_embed_data_from_file_null_path() {
    size_t buffer_size = 0;
    uint8_t *buffer = embed_data_from_file(NULL, &buffer_size);
    assert(buffer == NULL);
    assert(buffer_size == 0);
}

/**
 * @brief Test para embed_data_from_file con buffer_size NULL.
 *
 * Esta prueba verifica que la función retorna NULL cuando se le pasa un puntero buffer_size NULL.
 */
void test_embed_data_from_file_null_buffer_size() {
    const char *filename = "embed_test_null_buffer.txt";
    const char *file_content = "Data to embed into the buffer.";

    // Crear el archivo de prueba
    FILE *file = fopen(filename, "w");
    assert(file != NULL);
    fprintf(file, "%s", file_content);
    fclose(file);

    // Llamar a embed_data_from_file con buffer_size NULL
    uint8_t *buffer = embed_data_from_file(filename, NULL);
    assert(buffer == NULL);

    // Limpiar
    remove(filename);
}


int main() {
    set_log_level(NONE);

    // Pruebas de paquetes de archivos
    test_get_file_size();
    test_get_file_extension();
    test_create_file_package_valid();
    test_create_file_package_invalid();
    test_print_file_package();

    // Pruebas adicionales
//    test_new_file_package_from_data_valid();
//    test_new_file_package_from_data_invalid_size();
//    test_new_file_package_from_data_invalid_null();
    test_create_file_from_package_valid();
    test_create_file_from_package_invalid();
    test_create_file_from_raw_data_valid();
    test_create_file_from_raw_data_invalid();
    test_free_file_package_valid();
    test_free_file_package_null();

    test_embed_data_from_file_valid();
    test_embed_data_from_file_null_path();
    test_embed_data_from_file_null_buffer_size();

    printf("Todas las pruebas pasaron exitosamente.\n");
    return 0;
}
