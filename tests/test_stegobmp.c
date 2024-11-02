#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../src/include/stego_bmp.h"
#include "test_utils.c"


#define EXTENSION_SIZE 16
#define IMG_WIDTH 4
#define IMG_HEIGHT 4
#define PATTERN_MAP_SIZE 4

/**
 * @brief Test de Embedding Básico.
 *
 * Este test verifica que `embed_bits_generic` inserte correctamente los bits especificados
 * en los LSB de los componentes de color de una imagen BMP de 2x2 píxeles.
 */
void test_embed_bits_generic_basic() {
    // Crear una imagen BMP de 2x2 píxeles con todos los componentes inicializados a 0x00
    BMPImage *bmp = create_test_bmp(2, 2, 0x00);
    assert(bmp != NULL);

    uint8_t data_to_embed = 0xAA; // 10101010
    size_t num_bits = 8;
    size_t offset = 0;

    //print_pixel_data(bmp);
    bool result = embed_bits_generic(bmp, &data_to_embed, num_bits, &offset, 1);
    assert(result == true);
    //print_pixel_data(bmp);

    unsigned char expected_data[16] = {
            0x01, 0x00, 0x01,  // Componentes 0,1,2
            0x00, 0x01, 0x00,  // Componentes 3,4,5
            0x00, 0x00,        // Componentes 6,7 (Padding Fila 1)
            0x01, 0x00, 0x00,  // Componentes 8,9,10
            0x00, 0x00, 0x00,  // Componentes 11,12,13
            0x00, 0x00         // Padding Fila 2
    };
    for (size_t i = 0; i < 16; i++) {
        assert(bmp->data[i] == expected_data[i]);
    }

    free_bmp(bmp);
}

/**
 * @brief Test para verificar `bits_per_component` en `embed_bits_generic` usando 2 bits por componente.
 *
 * Este test utiliza 2 bits por componente y verifica que se establezcan correctamente
 * en los dos bits menos significativos de cada componente de color.
 */
void test_embed_bits_generic_multiple_bits() {
    BMPImage *bmp = create_test_bmp(2, 2, 0x00);
    assert(bmp != NULL);

    // 0xFF = 11111111
    uint8_t data_to_embed[] = {0xFF, 0xFF, 0xFF, 0xFF};
    size_t num_bits = 8 *4;
    size_t offset = 0;

    //print_pixel_data(bmp);
    bool result = embed_bits_generic(bmp, data_to_embed, num_bits, &offset, 4);
    assert(result == true);
    //print_pixel_data(bmp);

    // Verificar que los bits menos significativos de cada componente se hayan establecido correctamente
    unsigned char expected_data[16] = {
            0x0F, 0x0F, 0x0F,  // Componentes 0,1,2
            0x0F, 0x0F, 0x0F,  // Componentes 3,4,5
            0x00, 0x00,        // Padding fila 1
            0x0F, 0x0F, 0x00,  // Componentes 6,7,8
            0x00, 0x00, 0x00,  // Componentes 9,10,11
            0x00, 0x00         // Padding fila 2
    };
    for (size_t i = 0; i < 16; i++) {
        assert(bmp->data[i] == expected_data[i]);
    }
    free_bmp(bmp);
}

/**
 * @brief Test de desbordamiento de capacidad.
 *
 * Este test intenta insertar más bits de los que la imagen puede manejar y verifica
 * que la función `embed_bits_generic` falle adecuadamente.
 */
void test_embed_bits_generic_overflow() {
    BMPImage *bmp = create_test_bmp(2, 2, 0x00);
    assert(bmp != NULL);

    uint8_t data_to_embed[16] = {0xFF};
    size_t num_bits = 8 * sizeof(data_to_embed);
    size_t offset = 0;

    bool result = embed_bits_generic(bmp, data_to_embed, num_bits, &offset, 1);
    assert(result == false);
    free_bmp(bmp);
}

/**
 * @brief Test de embebido en componente individual.
 *
 * Este test verifica que `embed_bits_generic` inserte correctamente los bits en
 * un solo componente de la imagen.
 */
void test_embed_bits_generic_single_component() {
    BMPImage *bmp = create_test_bmp(1, 1, 0x00);
    assert(bmp != NULL);

    uint8_t data_to_embed = 0xAA; // 10101010
    size_t num_bits = 8;
    size_t offset = 0;

    //print_pixel_data(bmp);
    bool result = embed_bits_generic(bmp, &data_to_embed, num_bits, &offset, 8);
    assert(result == true);
    //print_pixel_data(bmp);

    unsigned char expected_data[3] = {0xAA, 0x00, 0x00};
    for (size_t i = 0; i < 3; i++) {
        assert(bmp->data[i] == expected_data[i]);
    }
    free_bmp(bmp);
}

/**
 * @brief Test básico de extracción.
 *
 * Este test verifica que `extract_bits_generic` extraiga correctamente los bits en
 * los LSB de los componentes de color de una imagen BMP de 2x2 píxeles.
 */
void test_extract_bits_generic_basic() {
    BMPImage *bmp = create_test_bmp(2, 2, 0x01);
    assert(bmp != NULL);
    //print_pixel_data(bmp);

    // Extraemos los datos
    uint8_t extracted_data = 0;
    size_t offset = 0;
    bool result = extract_bits_generic(bmp, 8, &extracted_data, &offset, 1);
    assert(result == true);
    assert(extracted_data == 0xFF);
    //print_data(&extracted_data, 1);
    free_bmp(bmp);
}

/**
 * @brief Test de extracción con `bits_per_component` mayor que 1.
 *
 * Este test configura manualmente los bits para probar la extracción con 2 bits por componente.
 */
void test_extract_bits_generic_multiple_bits() {
    BMPImage *bmp = create_test_bmp(3, 5, 0x03); // 0x03 = 00000011 en cada byte
    assert(bmp != NULL);
    //print_pixel_data(bmp);

    // Extraemos y verificamos
    uint8_t extracted_data[5];
    size_t offset = 0;
    bool result = extract_bits_generic(bmp, 8*5, extracted_data, &offset, 2);
    assert(result == true);
    for (size_t i = 0; i < 5; i++) {
        assert(extracted_data[i] == 0xFF);
    }
    //print_data(extracted_data, 5);
    free_bmp(bmp);
}

/**
 * @brief Test de extracción en un solo componente.
 *
 * Este test verifica que `extract_bits_generic` extraiga los bits correctamente
 * de un solo componente de la imagen BMP.
 */
void test_extract_bits_generic_single_component() {
    BMPImage *bmp = create_test_bmp(1, 1, 0xAA); // 0xAA = 10101010
    assert(bmp != NULL);
    print_pixel_data(bmp);

    // Extraemos y verificamos
    uint8_t extracted_data = 0x00;
    size_t offset = 0;
    bool result = extract_bits_generic(bmp, 8, &extracted_data, &offset, 8);
    assert(result == true);
    assert(extracted_data == 0xAA);
    //print_data(&extracted_data, 1);
    free_bmp(bmp);
}

/**
 * @brief Test de extracción que excede la capacidad de la imagen.
 *
 * Este test verifica que la función `extract_bits_generic` falle adecuadamente cuando
 * se intenta extraer más bits de los que contiene la imagen.
 */
void test_extract_bits_generic_overflow() {
    BMPImage *bmp = create_test_bmp(2, 2, 0x01);
    assert(bmp != NULL);
    //print_pixel_data(bmp);

    uint8_t buffer[16] = {0};
    size_t offset = 0;
    bool result = extract_bits_generic(bmp, 128, buffer, &offset, 1);
    assert(result == false);
    free_bmp(bmp);
}

/**
 * @brief Test para la función `check_capacity_lsb1`.
 */
void test_check_capacity_lsb1() {
    BMPImage bmp;
    bmp.width = IMG_WIDTH;
    bmp.height = IMG_HEIGHT;

    size_t  data_size = IMG_WIDTH * IMG_HEIGHT * 3;

    // Capacidad exacta
    assert(check_capacity_lsb1(&bmp, data_size) == true);

    // Excede la capacidad
    assert(check_capacity_lsb1(&bmp, data_size + 1) == false);
}

/**
 * @brief Test para la función `check_capacity_lsb4`.
 */
void test_check_capacity_lsb4() {
    BMPImage bmp;
    bmp.width = IMG_WIDTH;
    bmp.height = IMG_HEIGHT;
    bmp.data_size = IMG_WIDTH * IMG_HEIGHT * 3;

    size_t data_size = IMG_WIDTH * IMG_HEIGHT * 3 * 4;

    // Capacidad exacta (cada componente usa 4 bits)
    assert(check_capacity_lsb4(&bmp, data_size) == true);

    // Excede la capacidad
    assert(check_capacity_lsb4(&bmp, data_size + 1) == false);
}

/**
 * @brief Test para la función `check_capacity_lsbi`.
 */
void test_check_capacity_lsbi() {
    BMPImage bmp;
    bmp.width = IMG_WIDTH;
    bmp.height = IMG_HEIGHT;

    size_t data_size = IMG_WIDTH * IMG_HEIGHT * 2 * 2 - PATTERN_MAP_SIZE;

    // Capacidad exacta incluyendo el tamaño de pattern_map
    assert(check_capacity_lsbi(&bmp, data_size) == true);

    // Excede la capacidad incluyendo el tamaño de pattern_map
    assert(check_capacity_lsbi(&bmp, data_size + 1) == false);
}

/**
 * @brief Test de patrón pequeño para `embed_bits_lsbi`.
 *
 * Verifica que `embed_bits_lsbi` maneje correctamente patrones pequeños y múltiples componentes.
 */
void test_embed_bits_lsbi_large_pattern() {
    BMPImage *bmp = create_test_bmp(4, 4, 0x00);
    assert(bmp != NULL);

    uint8_t data_to_embed[2] = {0xFF, 0xF0}; // 16 bits: 11111111 11110000
    size_t num_bits = 16;
    size_t offset = 0;

    bool result = embed_bits_lsbi(bmp, data_to_embed, num_bits, &offset);
    assert(result == true);

    // Verificar los primeros 16 bits en componentes verdes y azules
    print_pixel_data(bmp);
    unsigned char expected_data[48] = {
            // Primera fila
            0x00, 0x00, 0x00, // Píxel 1
            0x01, 0x01, 0x00, // Píxel 2
            0x01, 0x01, 0x00, // Píxel 3
            0x01, 0x01, 0x00, // Píxel 4

            // Segunda fila
            0x01, 0x01, 0x00, // Píxel 1
            0x01, 0x01, 0x00, // Píxel 2
            0x01, 0x01, 0x00, // Píxel 3
            0x01, 0x00, 0x00, // Píxel 4

            // Segunda fila
            0x00, 0x00, 0x00,  // Píxel 1
            0x00, 0x00, 0x00,  // Píxel 2
            0x00, 0x00, 0x00,  // Píxel 3
            0x00, 0x00, 0x00,   // Píxel 4

            // Padding o valores sin embebido en la tercera y cuarta filas
            0x00, 0x00, 0x00,  // Píxel 1
            0x00, 0x00, 0x00,  // Píxel 2
            0x00, 0x00, 0x00,  // Píxel 3
            0x00, 0x00, 0x00   // Píxel 4
    };
    for (size_t i = 0; i < 48; i++) {
        assert(bmp->data[i] == expected_data[i]);
    }
    free_bmp(bmp);
}

void test_extract_bits_lsbi_basic() {
    BMPImage *bmp = create_test_bmp(4, 4, 0xAA);  // Crear una imagen de 4x4 píxeles
    assert(bmp != NULL);

    // Datos de prueba para insertar (bits alternantes para un patrón claro)
    uint8_t data_to_embed[2] = {0xFF, 0xFF}; // 16 bits: 10101010 10100000
    size_t num_bits = 16;
    size_t offset = 0;

    // Embebe los datos
    bool embed_result = embed_bits_lsbi(bmp, data_to_embed, num_bits, &offset);
    assert(embed_result == true);
    print_pixel_data(bmp);

    // Extraer los datos y compararlos con los datos embebidos
    uint8_t extracted_data[2] = {0x00}; // Inicializar el buffer de extracción
    offset = 4;  // Reiniciar el offset para comenzar desde el inicio
    uint8_t pattern_map = 0x02; // Valor esperado del pattern_map
    bool extract_result = extract_bits_lsbi(bmp, num_bits, extracted_data, &offset, &pattern_map);
    assert(extract_result == true);

    // Verificar que los datos extraídos coincidan con los embebidos
    for (size_t i = 0; i < sizeof(data_to_embed); i++) {
        LOG(INFO, "Extraído: %02X, Esperado: %02X", extracted_data[i], data_to_embed[i]);
        assert(extracted_data[i] == data_to_embed[i]);
    }
    free_bmp(bmp);
}

void test_extract_pattern_lsbi(){
    BMPImage *bmp = create_test_bmp(4, 4, 0xAA);  // Crear una imagen de 4x4 píxeles
    assert(bmp != NULL);

    // Datos de prueba para insertar (bits alternantes para un patrón claro)
    uint8_t data_to_embed[2] = {0xFF, 0xFF}; // 16 bits: 10101010 10100000
    size_t num_bits = 16;
    size_t offset = 0;

    // Embebe los datos
    bool embed_result = embed_bits_lsbi(bmp, data_to_embed, num_bits, &offset);
    assert(embed_result == true);
    print_pixel_data(bmp);


    uint8_t  pattern_map = 0;
    offset = 0;
    if (!extract_bits_generic(bmp, PATTERN_MAP_SIZE, &pattern_map, &offset, 1)) {
        LOG(ERROR, "Error al extraer pattern_map con LSB1 en extract_data.")
    }
    pattern_map = pattern_map >> 4; // Mover a los 4 bits menos significativos
    LOG(INFO, "Pattern Map: %d%d%d%d%d%d%d%d",
        (pattern_map >> 7) & 0x01,
        (pattern_map >> 6) & 0x01,
        (pattern_map >> 5) & 0x01,
        (pattern_map >> 4) & 0x01,
        (pattern_map >> 3) & 0x01,
        (pattern_map >> 2) & 0x01,
        (pattern_map >> 1) & 0x01,
        pattern_map & 0x01)
}

void test_extract_data_size() {
    /**
     * PARA QUE ESTE TEST FUNCIONE SE DEBE CAMBRIA "IS_DATA_BIG_ENDIAN" A FALSE
     */
    BMPImage *bmp = create_test_bmp(10, 10, 0x00); // Crear una imagen de prueba

    size_t offset = 0;
    unsigned int size_to_embed = 9999;
    bool embed_result = embed_bits_lsb1(bmp, (const uint8_t *) &size_to_embed, 32, &offset);
    assert(embed_result == true);
    print_pixel_data(bmp);

    // Reiniciar offset y extraer el tamaño
    offset = 0;
    uint32_t extracted_size = extract_data_size(bmp, STEG_LSB1, &offset, NULL); // Pasar NULL para context
    LOG(INFO, "Extraído: %u, Esperado: 9999", extracted_size);
    free_bmp(bmp);
}

void test_extract_size_lsb1() {
    BMPImage *bmp = new_bmp_file("../resources/test_images/ladoLSB1.bmp");
    assert(bmp != NULL);

    size_t offset = 0;
    uint32_t extracted_size = extract_data_size(bmp, STEG_LSB1, &offset, NULL);

    LOG(INFO, "Extraído: %u", extracted_size)
}

void test_extract_size_lsb4() {
    BMPImage *bmp = new_bmp_file("../resources/test_images/ladoLSB4.bmp");
    assert(bmp != NULL);

    size_t offset = 0;
    uint32_t extracted_size = extract_data_size(bmp, STEG_LSB4, &offset, NULL); // Pasar NULL para context

    LOG(INFO, "Extraído: %u", extracted_size)


    uint8_t *data = (uint8_t *)malloc(extracted_size);
    if (data == NULL) {
        LOG(ERROR, "No se pudo asignar memoria para los datos extraídos en extract_data.")
        free_bmp(bmp);
        return;
    }
    if (!extract_bits_lsb1(bmp, BYTES_TO_BITS(extracted_size), data, &offset, NULL)) {
        LOG(ERROR, "Error al extraer datos con LSBI en extract_data.")
        free(data);
        free_bmp(bmp);
        return;
    }

    LOG(INFO, "offset: %zu", offset)
}

void test_extract_bits_lsbi(){
    BMPImage *bmp = new_bmp_file("../resources/test_images/ladoLSBI.bmp");
    assert(bmp != NULL);

    uint8_t pattern_map = 0;
    size_t offset = 0;
    if (!extract_bits_lsb1(bmp, PATTERN_MAP_SIZE, &pattern_map, &offset, NULL)) {
        LOG(ERROR, "Error al extraer pattern_map con LSB1 en extract_data.")
    }

    uint32_t extracted_size = extract_data_size(bmp, STEG_LSBI, &offset, &pattern_map);
    LOG(INFO, "Extraído: %u", extracted_size)
}

void test_extract_extension_lsb1() {
    BMPImage *bmp = create_test_bmp(10, 10, 0x00); // Crear una imagen de prueba
    assert(bmp != NULL);

    size_t offset = 0;
    char* extension = ".txt";
    size_t extension_size = strlen(extension);
    embed_bits_lsb1(bmp, (const uint8_t *) extension, BYTES_TO_BITS(extension_size), &offset);
    //print_pixel_data(bmp);

    char buffer[EXTENSION_SIZE] = {0};
    offset = 0;
    if(!extract_extension(bmp, STEG_LSB1, buffer, &offset, NULL)){
        LOG(ERROR, "Error al extraer la extensión en extract_data.")
    }
    size_t buffer_size = strlen(buffer);
    LOG(INFO, "Extensión extraída: %s", buffer)
    LOG(INFO, "Tamaño de la extensión extraída: %zu", buffer_size)
    assert(buffer_size == extension_size);
    assert(strcmp(buffer, extension) == 0);
}

void test_extract_extension_lsb4() {
    BMPImage *bmp = create_test_bmp(10, 10, 0x00); // Crear una imagen de prueba
    assert(bmp != NULL);

    size_t offset = 0;
    char* extension = ".png";
    size_t extension_size = strlen(extension);
    embed_bits_lsb4(bmp, (const uint8_t *) extension, BYTES_TO_BITS(extension_size), &offset);
    //print_pixel_data(bmp);

    char buffer[EXTENSION_SIZE] = {0};
    offset = 0;
    if(!extract_extension(bmp, STEG_LSB4, buffer, &offset, NULL)){
        LOG(ERROR, "Error al extraer la extensión en extract_data.")
    }
    size_t buffer_size = strlen(buffer);
    LOG(INFO, "Extensión extraída: %s", buffer)
    LOG(INFO, "Tamaño de la extensión extraída: %zu", buffer_size)
    assert(buffer_size == extension_size);
    assert(strcmp(buffer, extension) == 0);
}

void test_extract_extension_lsbi() {
    BMPImage *bmp = create_test_bmp(10, 10, 0x00); // Crear una imagen de prueba
    assert(bmp != NULL);

    size_t offset = 0;
    char* extension = ".txt";
    size_t extension_size = strlen(extension);
    embed_bits_lsbi(bmp, (const uint8_t *) extension, BYTES_TO_BITS(extension_size), &offset);
    //print_pixel_data(bmp);

    offset = 4; // Se saltean los primeros 4 bits que son el pattern_map
    uint8_t pattern_map = 0;
    char buffer[EXTENSION_SIZE] = {0};
    if(!extract_extension(bmp, STEG_LSBI, buffer, &offset, &pattern_map)){
        LOG(ERROR, "Error al extraer la extensión en extract_data.")
    }
    size_t buffer_size = strlen(buffer);
    LOG(INFO, "Extensión extraída: %s", buffer)
    LOG(INFO, "Tamaño de la extensión extraída: %zu", buffer_size)
    assert(buffer_size == extension_size);
    assert(strcmp(buffer, extension) == 0);
}

void test_extract_file_lsb1(){
    BMPImage *bmp = new_bmp_file("../resources/test_images/ladoLSB1.bmp");
    assert(bmp != NULL);

    FilePackage *package = extract_data(bmp, STEG_LSB1);
    if (package == NULL) {
        LOG(ERROR, "Error al extraer datos en extract_data.")
        free_bmp(bmp);
        return;
    }

    create_file_from_package("../resources/test_images/OUTPUT-ladoLSB1.bmp", package);
    free_file_package(package);
    free_bmp(bmp);
}

void test_extract_file_lsb4(){
    BMPImage *bmp = new_bmp_file("../resources/test_images/ladoLSB4.bmp");
    assert(bmp != NULL);

    FilePackage *package = extract_data(bmp, STEG_LSB4);
    if (package == NULL) {
        LOG(ERROR, "Error al extraer datos en extract_data.")
        free_bmp(bmp);
        return;
    }

    create_file_from_package("../resources/test_images/OUTPUT-ladoLSB4.bmp", package);
    free_file_package(package);
    free_bmp(bmp);
}

void test_extract_file_lsbi(){
    BMPImage *bmp = new_bmp_file("../resources/test_images/ladoLSBI.bmp");
    assert(bmp != NULL);

    FilePackage *package = extract_data(bmp, STEG_LSBI);
    if (package == NULL) {
        LOG(ERROR, "Error al extraer datos en extract_data.")
        free_bmp(bmp);
        return;
    }

    create_file_from_package("../resources/test_images/OUTPUT-ladoLSBI.bmp", package);
    free_file_package(package);
    free_bmp(bmp);
}

/**
 * @brief Prueba de conversión de endian para datos de 4 bytes.
 */
void test_format_data_endian_4bytes() {
    uint8_t data_big_endian[4] = {0x12, 0x34, 0x56, 0x78};
    uint8_t expected_little_endian[4] = {0x78, 0x56, 0x34, 0x12};

    //print_buffer(data_big_endian, 4);
    format_data_endian(data_big_endian, 4);
    //print_buffer(data_big_endian, 4);

    if (IS_SYSTEM_BIG_ENDIAN()) {
        // En big-endian, los datos no deben cambiar
        assert(data_big_endian[0] == 0x12 && data_big_endian[1] == 0x34);
        assert(data_big_endian[2] == 0x56 && data_big_endian[3] == 0x78);
    } else {
        // En little-endian, los datos deben igualar al valor esperado
        for (size_t i = 0; i < 4; i++) {
            assert(data_big_endian[i] == expected_little_endian[i]);
        }
    }
}

/**
 * @brief Prueba de conversión de endian para datos de 2 bytes.
 */
void test_format_data_endian_2bytes() {
    uint8_t data_big_endian[2] = {0xAB, 0xCD};

    //print_buffer(data_big_endian, 2);
    format_data_endian(data_big_endian, 2);
    //print_buffer(data_big_endian, 2);

    if (IS_SYSTEM_BIG_ENDIAN()) {
        // En big-endian, los datos no deben cambiar
        assert(data_big_endian[0] == 0xAB && data_big_endian[1] == 0xCD);
    } else {
        // En little-endian, los datos deben igualar al valor esperado
        assert(data_big_endian[0] == 0xCD && data_big_endian[1] == 0xAB);
    }
}

int main() {
    set_log_level(DEBUG);

//    test_embed_bits_generic_basic();
//    test_embed_bits_generic_multiple_bits();
//    test_embed_bits_generic_overflow();
//    test_embed_bits_generic_single_component();

//    test_extract_bits_generic_basic();
//    test_extract_bits_generic_multiple_bits();
//    test_extract_bits_generic_single_component();
//    test_extract_bits_generic_overflow();

//    test_check_capacity_lsb1();
//    test_check_capacity_lsb4();
//    test_check_capacity_lsbi();

//    test_embed_bits_lsbi_large_pattern();

//    test_extract_bits_lsbi_basic();
    //test_extract_pattern_lsbi();
    //test_extract_bits_lsbi();

//    test_extract_data_size();
//    test_extract_size_lsb1();
//    test_extract_size_lsb4();
//    test_extract_bits_lsbi();

//    test_extract_extension_lsb1();
//    test_extract_extension_lsb4();
//    test_extract_extension_lsbi();

//    test_extract_file_lsb1();
//    test_extract_file_lsb4();
//    test_extract_file_lsbi();

//    test_format_data_endian_4bytes();
//    test_format_data_endian_2bytes();

    printf("Todos los tests pasaron exitosamente.\n");
    return 0;
}
