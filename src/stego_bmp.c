#include "stego_bmp.h"

#define HIDDEN_DATA_SIZE_FIELD 32   // Tamaño en bits del campo que almacena el tamaño de los datos ocultos
#define EXTENSION_SIZE 16           // Tamaño máximo permitido para la extensión del archivo
#define PATTERN_MAP_SIZE 4          // Tamaño en bits del mapa de patrones para LSBI


/**
 * @brief Funciones de esteganografía para ocultar y extraer datos en imágenes BMP.
 */
bool embed_bits_lsb1(BMPImage *bmp, const uint8_t *data, size_t num_bits, size_t *offset);
bool embed_bits_lsb4(BMPImage *bmp, const uint8_t *data, size_t num_bits, size_t *offset);
bool embed_bits_lsbi(BMPImage *bmp, const uint8_t *data, size_t num_bits, size_t *offset);
bool extract_bits_lsb1(const BMPImage *bmp, size_t num_bits, uint8_t *buffer, size_t *offset, void *context);
bool extract_bits_lsb4(const BMPImage *bmp, size_t num_bits, uint8_t *buffer, size_t *offset, void *context);
bool extract_bits_lsbi(const BMPImage *bmp, size_t num_bits, uint8_t *buffer, size_t *offset, void *context);
bool check_capacity_lsb1(const BMPImage *bmp, size_t num_bits);
bool check_capacity_lsb4(const BMPImage *bmp, size_t num_bits);
bool check_capacity_lsbi(const BMPImage *bmp, size_t num_bits);

/**
 * @brief Estructura que encapsula las operaciones de inserción y extracción de esteganografía.
 */
typedef struct {
    bool (*embed)(BMPImage *bmp, const uint8_t *data, size_t num_bits, size_t *offset);
    bool (*extract)(const BMPImage *bmp, size_t num_bits, uint8_t *buffer, size_t *offset, void *context);
    bool (*check_capacity)(const BMPImage *bmp, size_t num_bits);
} StegOperations;

static const StegOperations steg_operations[] = {
        [STEG_LSB1] = {
                .embed = embed_bits_lsb1,
                .extract = extract_bits_lsb1,
                .check_capacity = check_capacity_lsb1
        },
        [STEG_LSB4] = {
                .embed = embed_bits_lsb4,
                .extract = extract_bits_lsb4,
                .check_capacity = check_capacity_lsb4
        },
        [STEG_LSBI] = {
                .embed = embed_bits_lsbi,
                .extract = extract_bits_lsbi,
                .check_capacity = check_capacity_lsbi
        }
};


/**
 * @brief Declaracion de funciones privadas.
 */
bool embed_bits_generic(BMPImage *bmp, const uint8_t *data, size_t num_bits, size_t *offset, int bits_per_component);
bool extract_bits_generic(const BMPImage *bmp, size_t num_bits, uint8_t *buffer, size_t *offset, int bits_per_component);
void format_data_endian(uint8_t *data, size_t size);
size_t extract_data_size(const BMPImage *bmp, StegAlgorithm steg_alg, size_t *offset, void *context);
bool extract_extension(const BMPImage *bmp, StegAlgorithm steg_alg, char *ext_buffer, size_t *offset, void *context);
/****************************************
 ****  ALGORITMOS DE ESTEGANOGRAFÍA  ****
 ***************************************/
/**
 * @brief Inserta bits de datos en la imagen BMP utilizando un método LSB específico.
 *
 * @param bmp         Puntero a la estructura BMPImage.
 * @param data        Puntero a los datos que se desean insertar.
 * @param num_bits    Número de bits de datos a insertar.
 * @param offset      Puntero al índice desde donde comenzar a insertar los bits en bmp->data.
 *                    La función actualiza el valor de offset para continuar desde el fin de la operación.
 * @param bits_per_component Número de bits LSB a utilizar por componente.
 * @return bool       true si la inserción fue exitosa, false en caso de error.
 */
bool embed_bits_generic(BMPImage *bmp, const uint8_t *data, size_t num_bits, size_t *offset, int bits_per_component) {
    if (bmp == NULL || bmp->data == NULL || data == NULL || offset == NULL) {
        LOG(ERROR, "Argumentos NULL en embed_bits_generic.")
        return false;
    }

    size_t component_index = *offset;
    size_t bit_index = 0;

    for (; bit_index < num_bits; bit_index += bits_per_component) {
        if (component_index >= bmp->data_size) {
            LOG(ERROR, "No hay espacio suficiente en BMP para embebido de datos.")
            return false;
        }

        uint8_t byte = data[bit_index / 8];
        size_t start_bit_position = 7 - (bit_index % 8); // Posición inicial en el byte
        uint8_t bits_value = 0;

        // Extraemos los `bits_per_component` bits
        for (int bit = 0; bit < bits_per_component; bit++) {
            size_t current_bit_position = start_bit_position - bit;
            bits_value |= ((byte >> current_bit_position) & 0x01) << (bits_per_component - 1 - bit);
        }

        uint8_t *component = get_component_by_index(bmp, component_index++).component_ptr;
        if (component == NULL) {
            LOG(ERROR, "get_component_by_index retornó NULL en embed_bits_generic.")
            return false;
        }

        // Colocar los bits en `bits_per_component` menos significativos del componente
        *component = (*component & (~((1 << bits_per_component) - 1))) | bits_value;
    }
    *offset = component_index;
    return true;
}

/**
 * @brief Extrae bits de datos ocultos en la imagen BMP utilizando un método LSB específico.
 *
 * @param bmp               Puntero a la estructura BMPImage.
 * @param num_bits          Número de bits que se desean extraer.
 * @param buffer            Puntero al buffer donde se almacenarán los bits extraídos.
 * @param offset            Puntero al índice desde donde comenzar a extraer los bits en bmp->data.
 *                          La función actualiza el valor de offset para continuar desde el fin de la operación.
 * @param bits_per_component Número de bits LSB a utilizar por componente.
 * @param context           Contexto adicional necesario para la extracción (puede ser NULL).
 * @return bool             true si la extracción fue exitosa, false en caso de error.
 */
bool extract_bits_generic(const BMPImage *bmp, size_t num_bits, uint8_t *buffer, size_t *offset, int bits_per_component) {
    if (bmp == NULL || bmp->data == NULL || buffer == NULL || offset == NULL) {
        LOG(ERROR, "Argumentos NULL en extract_bits_generic.")
        return false;
    }

    size_t component_index = *offset;
    size_t bit_index = 0;

    // Inicializar el buffer a cero para evitar resultados inesperados en los bits no utilizados
    memset(buffer, 0, (num_bits + 7) / 8);

    for (; bit_index < num_bits;) {
        if (component_index >= bmp->data_size) {
            LOG(ERROR, "No hay suficiente espacio en BMP para extracción de datos.")
            return false;
        }

        // Extraer `bits_per_component` bits desde el componente
        Component component = get_component_by_index(bmp, component_index++);
        if (component.component_ptr == NULL) {
            LOG(ERROR, "get_component_by_index retornó NULL en extract_bits_generic.")
            return false;
        }

        uint8_t extracted_bits = *component.component_ptr & ((1 << bits_per_component) - 1);

        // Almacenar los bits extraídos en el buffer
        for (int bit = bits_per_component - 1; bit >= 0 && bit_index < num_bits; bit--, bit_index++) {
            uint8_t bit_value = (extracted_bits >> bit) & 0x01;
            buffer[bit_index / 8] |= (bit_value << (7 - (bit_index % 8)));
        }
    }

    *offset = component_index;
    return true;
}

/**
 * @brief Inserta bits de datos en la imagen BMP utilizando el algoritmo LSB1.
 *
 * @param bmp         Puntero a la estructura BMPImage.
 * @param data        Puntero a los datos que se desean insertar.
 * @param num_bits    Número de bits de datos a insertar.
 * @param offset      Puntero al índice desde donde comenzar a insertar los bits en bmp->data.
 *                    La función actualiza el valor de offset para continuar desde el fin de la operación.
 * @return bool       true si la inserción fue exitosa, false en caso de error.
 */
bool embed_bits_lsb1(BMPImage *bmp, const uint8_t *data, size_t num_bits, size_t *offset) {
    return embed_bits_generic(bmp, data, num_bits, offset, 1);
}

/**
 * @brief Extrae bits de datos ocultos en la imagen BMP utilizando el algoritmo LSB1.
 *
 * @param bmp         Puntero a la estructura BMPImage.
 * @param num_bits    Número de bits que se desean extraer.
 * @param buffer      Puntero al buffer donde se almacenarán los bits extraídos.
 * @param offset      Puntero al índice desde donde comenzar a extraer los bits en bmp->data.
 *                    La función actualiza el valor de offset para continuar desde el fin de la operación.
 * @param context     Contexto adicional necesario para la extracción (puede ser NULL).
 * @return bool       true si la extracción fue exitosa, false en caso de error.
 */
bool extract_bits_lsb1(const BMPImage *bmp, size_t num_bits, uint8_t *buffer, size_t *offset, void *context) {
    return extract_bits_generic(bmp, num_bits, buffer, offset, 1);
}

/**
 * @brief Inserta bits de datos en la imagen BMP utilizando el algoritmo LSB4.
 *
 * @param bmp         Puntero a la estructura BMPImage.
 * @param data        Puntero a los datos que se desean insertar.
 * @param num_bits    Número de bits de datos a insertar.
 * @param offset      Puntero al índice desde donde comenzar a insertar los bits en bmp->data.
 *                    La función actualiza el valor de offset para continuar desde el fin de la operación.
 * @return bool       true si la inserción fue exitosa, false en caso de error.
 */
bool embed_bits_lsb4(BMPImage *bmp, const uint8_t *data, size_t num_bits, size_t *offset) {
    if (num_bits % 4 != 0) {
        LOG(ERROR, "num_bits debe ser múltiplo de 4 en embed_bits_lsb4.")
        return false;
    }
    return embed_bits_generic(bmp, data, num_bits, offset, 4);
}

/**
 * @brief Extrae bits de datos ocultos en la imagen BMP utilizando el algoritmo LSB4.
 *
 * @param bmp         Puntero a la estructura BMPImage.
 * @param num_bits    Número de bits que se desean extraer.
 * @param buffer      Puntero al buffer donde se almacenarán los bits extraídos.
 * @param offset      Puntero al índice desde donde comenzar a extraer los bits en bmp->data.
 *                    La función actualiza el valor de offset para continuar desde el fin de la operación.
 * @param context     Contexto adicional necesario para la extracción (puede ser NULL).
 * @return bool       true si la extracción fue exitosa, false en caso de error.
 */
bool extract_bits_lsb4(const BMPImage *bmp, size_t num_bits, uint8_t *buffer, size_t *offset, void *context) {
    if (num_bits % 4 != 0) {
        LOG(ERROR, "num_bits debe ser múltiplo de 4 en extract_bits_lsb4.")
        return false;
    }
    return extract_bits_generic(bmp, num_bits, buffer, offset, 4);
}

/**
 * @brief Inserta bits de datos en la imagen BMP utilizando el algoritmo LSBI.
 *
 * @param bmp         Puntero a la estructura BMPImage.
 * @param data        Puntero a los datos que se desean insertar.
 * @param num_bits    Número de bits de datos a insertar.
 * @param offset      Puntero al índice desde donde comenzar a insertar los bits en bmp->data.
 *                    La función actualiza el valor de offset para continuar desde el fin de la operación.
 * @return bool       true si la inserción fue exitosa, false en caso de error.
 */
bool embed_bits_lsbi(BMPImage *bmp, const uint8_t *data, size_t num_bits, size_t *offset) {
    if (bmp == NULL || bmp->data == NULL || data == NULL || offset == NULL) {
        LOG(ERROR, "Argumentos NULL en embed_bits_lsbi.")
        return false;
    }

    size_t component_index = *offset + PATTERN_MAP_SIZE; // 4 bits para pattern_map usando LSB1
    size_t bit_to_embed_count = 0;
    size_t pattern_changed[PATTERN_MAP_SIZE] = {0};
    size_t pattern_unchanged[PATTERN_MAP_SIZE] = {0};

    if (component_index + num_bits > bmp->data_size) {
        LOG(ERROR, "No hay suficiente espacio para embeber datos y pattern_map.")
        return false;
    }

    // Paso 1: Insertar datos y contar cambios
    for (; component_index < bmp->width * bmp->height * 3 && bit_to_embed_count < num_bits; component_index++) {
        Component comp = get_component_by_index(bmp, component_index);
        if (comp.component_ptr == NULL || (comp.color != GREEN && comp.color != BLUE)) continue;

        uint8_t original_component = *comp.component_ptr;
        uint8_t pattern = (original_component >> 1) & 0x03;

        uint8_t bit = (data[bit_to_embed_count / 8] >> (7 - (bit_to_embed_count % 8))) & 0x01;

        *comp.component_ptr = (original_component & 0xFE) | bit;

        if (*comp.component_ptr != original_component) {
            pattern_changed[pattern]++;
        } else {
            pattern_unchanged[pattern]++;
        }

        bit_to_embed_count++;
    }

    // Verificar si todos los bits fueron embebidos
    if (bit_to_embed_count < num_bits) {
        LOG(ERROR, "No se pudieron embeber todos los bits de datos.")
        return false;
    }

    // Paso 2: Construir pattern_map
    uint8_t pattern_map = 0;
    for (int p = 0; p < PATTERN_MAP_SIZE; p++) {
        if (pattern_changed[p] > pattern_unchanged[p]) {
            pattern_map |= (1 << p);
        }
    }
    LOG(INFO, "Pattern Map: %d%d%d%d%d%d%d%d",
        (pattern_map >> 7) & 0x01,
        (pattern_map >> 6) & 0x01,
        (pattern_map >> 5) & 0x01,
        (pattern_map >> 4) & 0x01,
        (pattern_map >> 3) & 0x01,
        (pattern_map >> 2) & 0x01,
        (pattern_map >> 1) & 0x01,
        pattern_map & 0x01)

    // Paso 3: Embeber pattern_map en los primeros 4 componentes usando LSB1
    size_t pattern_map_offset = *offset;
    uint8_t pattern_map_to_embed = pattern_map << 4; // Mover a los 4 bits más significativos
    if (!steg_operations[STEG_LSB1].embed(bmp, &pattern_map_to_embed, PATTERN_MAP_SIZE, &pattern_map_offset)) {
        LOG(ERROR, "Error al embeber pattern_map.")
        return false;
    }

    // Paso 4: Aplicar inversión de LSB1 según pattern_map
    component_index = *offset + PATTERN_MAP_SIZE;
    bit_to_embed_count = 0;
    for (; component_index < bmp->data_size && bit_to_embed_count < num_bits; component_index++) {
        Component comp = get_component_by_index(bmp, component_index);
        if (comp.component_ptr == NULL || (comp.color != GREEN && comp.color != BLUE)) {
            continue;
        }

        uint8_t pattern = (*comp.component_ptr >> 1) & 0x03;

        // Solo invertir si el patrón está marcado y el LSB actualmente no coincide
        if ((pattern_map & (1 << pattern)) && ((*comp.component_ptr & 0x01) != ((data[bit_to_embed_count / 8] >> (7 - (bit_to_embed_count % 8))) & 0x01))) {
            *comp.component_ptr ^= 0x01;
        }

        bit_to_embed_count++;
    }

    // Actualizar el offset para futuras operaciones
    *offset = component_index;
    return true;
}

/**
 * @brief Extrae bits de datos ocultos en la imagen BMP utilizando el algoritmo LSBI.
 *
 * @param bmp         Puntero a la estructura BMPImage.
 * @param num_bits    Número de bits que se desean extraer.
 * @param buffer      Puntero al buffer donde se almacenarán los bits extraídos.
 * @param offset      Puntero al índice desde donde comenzar a extraer los bits en bmp->data.
 *                    La función actualiza el valor de offset para continuar desde el fin de la operación.
 * @param context     Contexto adicional necesario para la extracción (en este caso, el `pattern_map`).
 * @return bool       true si la extracción fue exitosa, false en caso de error.
 */
bool extract_bits_lsbi(const BMPImage *bmp, size_t num_bits, uint8_t *buffer, size_t *offset, void *context) {
    if (bmp == NULL || bmp->data == NULL || buffer == NULL || offset == NULL || context == NULL) {
        LOG(ERROR, "Argumentos NULL en extract_bits_lsbi.");
        return false;
    }

    // Inicializar el buffer para evitar valores residuales
    memset(buffer, 0, (num_bits + 7) / 8);

    size_t max_component_index = bmp->width * bmp->height * 3; // Evita salirse del rango
    size_t component_index = *offset;
    size_t bit_extracted_count = 0;
    uint8_t pattern_map = *((uint8_t *)context) >> 4; // Mover a los 4 bits más significativos
//    LOG(INFO, "Pattern Map: %d%d%d%d%d%d%d%d",
//        (pattern_map >> 7) & 0x01,
//        (pattern_map >> 6) & 0x01,
//        (pattern_map >> 5) & 0x01,
//        (pattern_map >> 4) & 0x01,
//        (pattern_map >> 3) & 0x01,
//        (pattern_map >> 2) & 0x01,
//        (pattern_map >> 1) & 0x01,
//        pattern_map & 0x01);

    // Extraer los datos embebidos y aplicar la inversión si es necesario
    for (; component_index < max_component_index && bit_extracted_count < num_bits; component_index++) {

        Component comp = get_component_by_index(bmp, component_index);
        if (comp.component_ptr == NULL || (comp.color != GREEN && comp.color != BLUE)) {
            continue; // Saltar los componentes que no son verde o azul
        }

        uint8_t component = *comp.component_ptr;
        uint8_t pattern = (component >> 1) & 0x03;

        // Verificar si este patrón fue invertido usando el pattern_map
        if ((pattern_map & (1 << pattern)) != 0) {  // Aseguramos solo invertir si el patrón está activado
            //LOG(DEBUG, "Inverting bit for component %zu, pattern: %02X, original: %02X", component_index, pattern, component);
            component ^= 0x01; // Invertir el LSB
        }
        // Extraer el LSB y almacenar en el buffer de bits
        uint8_t bit = component & 0x01;
        buffer[bit_extracted_count / 8] |= (bit << (7 - (bit_extracted_count % 8)));
        bit_extracted_count++;
    }

    // Verificar que se hayan extraído el número total de bits esperado
    if (bit_extracted_count != num_bits) {
        LOG(ERROR, "No se extrajeron todos los bits requeridos.")
        return false;
    }

    // Actualizar el offset para la próxima operación
    *offset = component_index;
    return true;
}

/******************************
 ****  FUNCIONES PRIVADAS  ****
 *****************************/
/**
* @brief Verifica si la imagen BMP tiene suficiente capacidad para embeber los bits usando LSB1.
*
* @param bmp       Puntero a la estructura BMPImage.
* @param num_bits  Número de bits que se desean embeber.
* @return bool     true si hay suficiente capacidad, false en caso contrario.
*/
bool check_capacity_lsb1(const BMPImage *bmp, size_t num_bits) {
    if (bmp == NULL) {
        LOG(ERROR, "BMPImage NULL en check_capacity_lsb1.")
        return false;
    }
    // por cada componente puedo almacenar un bit
    return bmp->width * bmp->height * 3 * 1 >= num_bits;
}

/**
 * @brief Verifica si la imagen BMP tiene suficiente capacidad para embeber los bits usando LSB4.
 *
 * @param bmp       Puntero a la estructura BMPImage.
 * @param num_bits  Número de bits que se desean embeber.
 * @return bool     true si hay suficiente capacidad, false en caso contrario.
 */
bool check_capacity_lsb4(const BMPImage *bmp, size_t num_bits) {
    if (bmp == NULL) {
        LOG(ERROR, "BMPImage NULL en check_capacity_lsb4.")
        return false;
    }

    // Cada componente almacena 4 bits
    return bmp->width * bmp->height * 3 * 4 >= num_bits;
}

/**
 * @brief Verifica si la imagen BMP tiene suficiente capacidad para embeber los bits usando LSBI.
 *
 * @param bmp       Puntero a la estructura BMPImage.
 * @param num_bits  Número de bits de datos que se desean embeber.
 * @return bool     true si hay suficiente capacidad, false en caso contrario.
 */
bool check_capacity_lsbi(const BMPImage *bmp, size_t num_bits) {
    if (bmp == NULL) {
        LOG(ERROR, "BMPImage NULL en check_capacity_lsbi.")
        return false;
    }

    // Solo uso dos componentes por pixel y por cada uno almaceno 2 bit
    return bmp->width * bmp->height * 2 * 2 >= num_bits + PATTERN_MAP_SIZE;
}

/**
 * @brief Convierte los datos de un buffer a la representación endian correcta.
 *
 * @param data       Puntero a los datos extraídos.
 * @param size       Tamaño de los datos en bytes.
 */
void format_data_endian(uint8_t *data, size_t size) {
    if (IS_SYSTEM_BIG_ENDIAN() != IS_DATA_BIG_ENDIAN) {
        LOG(DEBUG, "Your system is %s-endian, data is %s-endian. Converting data endian.",
            IS_SYSTEM_BIG_ENDIAN() ? "big" : "little", IS_DATA_BIG_ENDIAN ? "big" : "little")
        for (size_t i = 0; i < size / 2; i++) {
            uint8_t temp = data[i];
            data[i] = data[size - i - 1];
            data[size - i - 1] = temp;
        }
    }
}

/**
 * @brief Extrae el tamaño de los datos ocultos en bits en la imagen BMP.
 *
 * @param bmp           Puntero a la estructura BMPImage.
 * @param steg_alg      Algoritmo de esteganografía a utilizar.
 * @param offset        Índice de inicio para la extracción en bmp->data.
 *                      La función actualiza el valor de offset para continuar desde el fin de la operación.
 * @param context       Contexto adicional necesario para la extracción.
 * @return size_t       Tamaño de los datos extraídos, o 0 en caso de error.
 */
size_t extract_data_size(const BMPImage *bmp, StegAlgorithm steg_alg, size_t *offset, void *context) {
    if (bmp == NULL || bmp->data == NULL || offset == NULL || (steg_alg == STEG_LSBI && context == NULL)) {
        LOG(ERROR, "Argumentos inválidos en extract_data_size.")
        return 0;
    }

    uint32_t extracted_size = 0;
    if (!steg_operations[steg_alg].extract(bmp, HIDDEN_DATA_SIZE_FIELD, (uint8_t *)&extracted_size, offset, context)) {
        LOG(ERROR, "Error al extraer el tamaño de los datos.")
        return 0;
    }

    // Convertir el valor si es necesario
    format_data_endian((uint8_t *)&extracted_size, sizeof(extracted_size));

    LOG(INFO, "Tamaño de datos extraído: %u", extracted_size)
    return extracted_size;
}


/**
 * @brief Extrae la extensión del archivo embebido en la imagen BMP.
 *
 * @param bmp           Puntero a la estructura BMPImage.
 * @param steg_alg      Algoritmo de esteganografía a utilizar.
 * @param ext_buffer    Buffer donde se almacenará la extensión extraída.
 * @param offset        Índice inicial desde donde comenzar la extracción en bmp->data.
 *                      La función actualiza el valor de offset para continuar desde el fin de la operación.
 * @param context       Contexto adicional necesario para la extracción (puede ser NULL).
 * @return bool         true si la extensión se extrajo correctamente, false en caso contrario.
 */
bool extract_extension(const BMPImage *bmp, StegAlgorithm steg_alg, char *ext_buffer, size_t *offset, void *context) {
    if (bmp == NULL || bmp->data == NULL || ext_buffer == NULL || offset == NULL || (steg_alg == STEG_LSBI && context == NULL)) {
        LOG(ERROR, "Argumentos NULL en extract_extension.")
        return false;
    }

    size_t ext_index = 0;
    uint8_t byte;
    while (ext_index < EXTENSION_SIZE) {
        // Extraer un byte de la extensión
        if (!steg_operations[steg_alg].extract(bmp, BYTES_TO_BITS(1), &byte, offset, context)) {
            LOG(ERROR, "Error al extraer la extensión del archivo.")
            return false;
        }

        ext_buffer[ext_index++] = (char) byte;

        // Verificar si se ha alcanzado el final de la extensión
        if (byte == '\0') break;
    }

    // Verificamos si comienza con "." y si termina con "\0"
    if (ext_buffer[0] != '.' || ext_buffer[ext_index - 1] != '\0') {
        ext_buffer[ext_index - 1] = '\0'; // Asegurar terminación
        LOG(ERROR, "Extensión de archivo inválida: %s.", ext_buffer)
        return false;
    }

    return true;
}

/******************************
 ****  FUNCIONES PUBLICAS  ****
 *****************************/

bool embed(BMPImage *bmp, const uint8_t *secret_data, size_t secret_size, StegAlgorithm steg_alg) {
    if (bmp == NULL ||  bmp->data == NULL || secret_data == NULL) {
        LOG(ERROR, "Argumentos NULL en embed.")
        return false;
    }
    size_t offset = 0;
    size_t num_bits = BYTES_TO_BITS(secret_size);

    // Verificar la capacidad antes de proceder
    if (!steg_operations[steg_alg].check_capacity(bmp, num_bits)) {
        LOG(ERROR, "No hay suficiente capacidad para embeber los datos con el algoritmo especificado.")
        return false;
    }

    // Embedded
    if(!steg_operations[steg_alg].embed(bmp, secret_data, num_bits, &offset)){
        LOG(ERROR, "Error al embeber datos con el algoritmo especificado.")
        return false;
    }

    return true;
}

FilePackage* extract_data(const BMPImage *bmp, StegAlgorithm steg_alg) {
    if (bmp == NULL) {
        LOG(ERROR, "BMPImage NULL en extract_data.")
        return NULL;
    }

    // Crear un FilePackage para almacenar los datos extraídos
    FilePackage *package = (FilePackage *)malloc(sizeof(FilePackage));
    if (package == NULL) {
        LOG(ERROR, "No se pudo asignar memoria para FilePackage en extract_data.")
        return NULL;
    }
    package->size = 0;
    package->data = NULL;
    package->extension = NULL;

    size_t offset = 0;
    uint8_t pattern_map = 0;

    // Si el algoritmo es LSBI, primero extraer el pattern_map usando LSB1
    if(steg_alg == STEG_LSBI){
        if (!steg_operations[STEG_LSB1].extract(bmp, PATTERN_MAP_SIZE, &pattern_map, &offset, NULL)) {
            LOG(ERROR, "Error al extraer pattern_map con LSB1 en extract_data.")
            free_file_package(package);
            return NULL;
        }
        LOG(DEBUG, "Patron obtenido: %d%d%d%d%d%d%d%d",
            (pattern_map >> 7) & 0x01,
            (pattern_map >> 6) & 0x01,
            (pattern_map >> 5) & 0x01,
            (pattern_map >> 4) & 0x01,
            (pattern_map >> 3) & 0x01,
            (pattern_map >> 2) & 0x01,
            (pattern_map >> 1) & 0x01,
            pattern_map & 0x01)
    }

    // Extraer el tamaño del archivo
    package->size = extract_data_size(bmp, steg_alg, &offset, &pattern_map);
    if (package->size == 0) {
        LOG(ERROR, "Error al extraer tamaño de los datos en extract_data.")
        free_file_package(package);
        return NULL;
    }
    LOG(INFO, "Tamaño de los datos extraídos: %u bytes.", package->size)

    // Extraer los datos del archivo
    package->data = (uint8_t *)malloc(package->size);
    if (package->data == NULL) {
        LOG(ERROR, "No se pudo asignar memoria para los datos en extract_data.")
        free_file_package(package);
        return NULL;
    }
    if (!steg_operations[steg_alg].extract(bmp, BYTES_TO_BITS(package->size), package->data, &offset, &pattern_map)) {
        LOG(ERROR, "Error al extraer datos con el algoritmo especificado en extract_data.")
        free_file_package(package);
        return NULL;
    }

    // Extraer la extensión del archivo
    package->extension = (uint8_t *)malloc(EXTENSION_SIZE);
    if (package->extension == NULL) {
        LOG(ERROR, "No se pudo asignar memoria para la extensión en extract_data.")
        free_file_package(package);
        return NULL;
    }
    if (!extract_extension(bmp, steg_alg, (char*)package->extension, &offset, &pattern_map)) {
        LOG(ERROR, "Error al extraer la extensión en extract_data.")
        free_file_package(package);
        return NULL;
    }
    LOG(INFO, "Extensión extraída: %s.", package->extension)

    return package;
}


uint8_t* extract_encrypted_data(const BMPImage *bmp, StegAlgorithm steg_alg, size_t *extracted_size) {
    if (bmp == NULL || bmp->data == NULL || extracted_size == NULL) {
        LOG(ERROR, "Argumentos NULL en extract_encrypted_data.")
        return NULL;
    }

    size_t offset = 0;
    uint8_t pattern_map = 0;
    uint8_t* encrypted_data = NULL;
    *extracted_size = 0;

    // Si el algoritmo es LSBI, primero extraemos el pattern_map usando LSB1
    if (steg_alg == STEG_LSBI) {
        if (!steg_operations[STEG_LSB1].extract(bmp, PATTERN_MAP_SIZE, &pattern_map, &offset, NULL)) {
            LOG(ERROR, "Error al extraer pattern_map con LSB1 en extract_encrypted_data.")
            return NULL;
        }
        LOG(INFO, "Pattern Map: %02X", pattern_map)
    }

    // Extraer el tamaño cifrado (4 bytes)
    uint32_t encrypted_size = 0;
    if (!steg_operations[steg_alg].extract(bmp, HIDDEN_DATA_SIZE_FIELD, (uint8_t *)&encrypted_size, &offset, &pattern_map)) {
        LOG(ERROR, "Error al extraer tamaño cifrado con el algoritmo especificado en extract_encrypted_data.")
        return NULL;
    }
    // Verificar que el tamaño cifrado sea razonable
    if (encrypted_size == 0 || encrypted_size > bmp->data_size) {
        LOG(ERROR, "Tamaño cifrado inválido: %u bytes.", encrypted_size)
        return NULL;
    }
    LOG(INFO, "Tamaño de los datos cifrados: %u bytes.", encrypted_size)

    // Extraer los datos cifrados
    size_t data_bits = BYTES_TO_BITS(encrypted_size);
    encrypted_data = (uint8_t *)malloc(encrypted_size);
    if (encrypted_data == NULL) {
        LOG(ERROR, "No se pudo asignar memoria para los datos cifrados en extract_encrypted_data.")
        return NULL;
    }
    if (!steg_operations[steg_alg].extract(bmp, data_bits, encrypted_data, &offset, &pattern_map)) {
        LOG(ERROR, "Error al extraer datos cifrados con el algoritmo especificado en extract_encrypted_data.")
        free(encrypted_data);
        return NULL;
    }
    LOG(INFO, "Datos cifrados extraídos correctamente.")

    *extracted_size = encrypted_size;
    return encrypted_data;
}