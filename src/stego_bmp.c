#include "./include/stego_bmp.h"

void embed_LSB1(const BMPImage *bitmap, const FilePackage *file);
void embed_LSB4(const BMPImage *bitmap, const FilePackage *file);
void embed_LSBI(const BMPImage *bitmap, const FilePackage *file, const uint8_t *pattern);

FilePackage* extract_LSB1(const BMPImage *bitmap);
FilePackage* extract_LSB4(const BMPImage *bitmap);
FilePackage* extract_LSBI(const BMPImage *bitmap);

BMPImage* embed(BMPImage *bitmap, FilePackage *message, const StegAlgorithm steg_algorithm){
    //The possible steganography algorithms are:
    // LSB1, LSB4, LSBI
    // The embedded file will be returned as a BMPImage struct
    // The message is the file to be embedded
    // The steg_algorithm is the steganography algorithm to use


    // BMPImage* new_bmp = copy_bmp(bitmap);


    //TODO: Pattern should be designated by the user????
    uint8_t pattern[2] = {0b10, 0b10};  // Representación de 1010 en binario


    // Select the appropriate embedding function based on the steg_algorithm
    switch (steg_algorithm) {
    case STEG_LSB1:
        embed_LSB1(bitmap, message);
        break;
    case STEG_LSB4:
        embed_LSB4(bitmap, message);
        break;
    case STEG_LSBI:
        embed_LSBI(bitmap, message, pattern);
        break;
    default:
        LOG(ERROR, "Invalid steganography algorithm.")
        break;
    }

    return bitmap;
}
void embed_LSB1(const BMPImage *bitmap, const FilePackage *file) {
    // Check if the file is too large to embed in the bitmap
    const size_t total_bits_needed = (file->size * 8) + 32 + (strlen(file->extension) + 1) * 8;  // Include 32 bits for size and bits for extension
    if (total_bits_needed > bitmap->data_size) {
        LOG(ERROR, "File is too large to embed in the bitmap. Bitmap size is: %zu", bitmap->data_size)
        return;
    }

    // Embed the size of the file (first 4 bytes)
    const uint32_t file_size = file->size;
    for (size_t i = 0; i < 32; i++) {
        bitmap->data[i] = (bitmap->data[i] & 0xFE) | ((file_size >> (31 - i)) & 0x01);  // LSB embedding
    }

    // Embed the file data into the bitmap using the LSB1 algorithm (starting after the 4 bytes for size)
    for (size_t i = 0; i < file->size; i++) {
        unsigned char byte = file->data[i];
        for (int bit = 0; bit < 8; bit++) {
            bitmap->data[32 + i * 8 + bit] = (bitmap->data[32 + i * 8 + bit] & 0xFE) | ((byte >> (7 - bit)) & 0x01);
        }
    }

    // Embed the file extension (including the '.' and '\0')
    const size_t extension_offset = 32 + file->size * 8;
    for (size_t i = 0; i <= strlen(file->extension); i++) {  // Including '\0', so use <=
        const unsigned char ext_char = file->extension[i];
        for (int bit = 0; bit < 8; bit++) {
            bitmap->data[extension_offset + i * 8 + bit] = (bitmap->data[extension_offset + i * 8 + bit] & 0xFE) | ((ext_char >> (7 - bit)) & 0x01);
        }
    }

    LOG(INFO, "File, size, and extension embedded using LSB1 algorithm.")
}

void embed_LSB4(const BMPImage *bitmap, const FilePackage *file) {
    // Check if the file is too large to embed in the bitmap
    if (file->size * 2 > bitmap->data_size) {
        LOG(ERROR, "File is too large to embed in the bitmap. Bitmap size is: %zu", bitmap->data_size)
        return;
    }

    // Embed the size of the file (first 32 bits using 4 bits per byte)
    const uint32_t file_size = file->size;
    for (size_t i = 0; i < 8; i++) {  // 8 BMP bytes to store 32 bits of the file size
        unsigned char byte = (file_size >> (28 - i * 4)) & 0x0F;  // Get the 4 most significant bits per iteration
        bitmap->data[i] = (bitmap->data[i] & 0xF0) | byte;  // Set the 4 LSBs
    }

    // Embed the file data into the bitmap using the LSB4 algorithm (starting after the 4 bytes for size)
    size_t data_offset = 8;  // We've already used 8 bytes for the file size
    for (size_t i = 0; i < file->size; i++) {
        unsigned char byte = file->data[i];
        for (int nibble = 0; nibble < 2; nibble++) {  // Two nibbles per byte
            unsigned char nibble_data = (nibble == 0) ? (byte >> 4) : (byte & 0x0F);  // Get upper and lower nibbles
            bitmap->data[data_offset++] = (bitmap->data[data_offset] & 0xF0) | nibble_data;  // Embed the nibble
        }
    }

    // Embed the file extension (after the file data, until the null terminator '\0')
    const size_t extension_offset = data_offset;
    for (size_t i = 0; i <= strlen(file->extension); i++) {  // Including '\0', so use <=
        unsigned char ext_char = file->extension[i];
        for (int nibble = 0; nibble < 2; nibble++) {  // Two nibbles per character
            unsigned char nibble_data = (nibble == 0) ? (ext_char >> 4) : (ext_char & 0x0F);  // Get upper and lower nibbles
            bitmap->data[extension_offset + i * 2 + nibble] = (bitmap->data[extension_offset + i * 2 + nibble] & 0xF0) | nibble_data;
        }
    }

    LOG(INFO, "File and extension embedded using LSB4 algorithm.")
}

void embed_LSBI(const BMPImage *bitmap, const FilePackage *file, const uint8_t *pattern) {
    // Check if the file is too large to embed in the bitmap
    if ((file->size * 8 + 32 + 4) > bitmap->data_size) {  // Incluye 32 bits para el tamaño y 4 bits para los patrones
        LOG(ERROR, "File is too large to embed in the bitmap.")
        return;
    }

    // Embed the 4-bit change patterns (in the first 4 bytes, LSB1)
    for (size_t i = 0; i < 4; i++) {
        bitmap->data[i] = (bitmap->data[i] & 0xFE) | ((pattern[i / 2] >> ((1 - i % 2) * 2)) & 0x01);
    }

    // Embed the size of the file (next 32 bits, LSB1)
    const uint32_t file_size = file->size;
    for (size_t i = 4; i < 36; i++) {  // Start after the 4 bits for patterns
        bitmap->data[i] = (bitmap->data[i] & 0xFE) | ((file_size >> (35 - i)) & 0x01);
    }

    // Embed the file data into the bitmap using LSB1 (after the 36 bits for size and patterns)
    size_t data_start = 36;
    for (size_t i = 0; i < file->size; i++) {
        const unsigned char byte = file->data[i];
        for (int bit = 0; bit < 8; bit++) {
            bitmap->data[data_start + i * 8 + bit] = (bitmap->data[data_start + i * 8 + bit] & 0xFE) | ((byte >> (7 - bit)) & 0x01);
        }
    }

    // Embed the file extension (after the file data, until the null terminator '\0')
    const size_t extension_offset = data_start + file->size * 8;
    for (size_t i = 0; i <= strlen(file->extension); i++) {  // Including '\0', so use <=
        const unsigned char ext_char = file->extension[i];
        for (int bit = 0; bit < 8; bit++) {
            bitmap->data[extension_offset + i * 8 + bit] = (bitmap->data[extension_offset + i * 8 + bit] & 0xFE) | ((ext_char >> (7 - bit)) & 0x01);
        }
    }

    LOG(INFO, "File, size, extension, and patterns embedded using LSBI algorithm.")
}


FilePackage* extract(BMPImage* bmp_embed, const StegAlgorithm steg_algorithm){
    // The possible steganography algorithms are:
    // LSB1, LSB4, LSBI
    // The extracted file will be returned as a FilePackage struct
    // The steg_algorithm is the steganography algorithm to use

    FilePackage* file = NULL;
    // Select the appropriate extraction function based on the steg_algorithm
    switch (steg_algorithm) {
    case STEG_LSB1:
        file = extract_LSB1(bmp_embed);
        break;
    case STEG_LSB4:
        file = extract_LSB4(bmp_embed);
        break;
    case STEG_LSBI:
        file = extract_LSBI(bmp_embed);
        break;
    default:
        LOG(ERROR, "Invalid steganography algorithm.")
        break;
    }


    return file;
}

FilePackage* extract_LSB1(const BMPImage *bitmap) {

    FilePackage *file = malloc(sizeof(FilePackage));
    if (!file) {
        LOG(ERROR, "Memory allocation failed.")
        return NULL;
    }

    // Extract the size of the file (first 32 bits)
    file->size = 0;
    for (size_t i = 0; i < 32; i++) {
        file->size = (file->size << 1) | (bitmap->data[i] & 0x01);
    }
    LOG(INFO, "Extracted file size: %zu", file->size)

    // Allocate memory for the file data
    file->data = malloc(file->size);
    if (!file->data) {
        LOG(ERROR, "Memory allocation for file data failed.")
        free(file);
        return NULL;
    }

    // Extract the file data from the bitmap using the LSB1 algorithm (after the 4 bytes for size)
    for (size_t i = 0; i < file->size; i++) {
        unsigned char byte = 0;
        for (int bit = 0; bit < 8; bit++) {
            byte = (byte << 1) | (bitmap->data[32 + i * 8 + bit] & 0x01);
        }
        file->data[i] = byte;
    }

    LOG(INFO, "File data extracted.")

    // Extract the file extension (after the file data, until the null terminator '\0')
    const size_t extension_offset = 32 + file->size * 8;
    char *extension = malloc(16);  // Assuming max 16 characters for the extension, including '.\0'
    if (!extension) {
        LOG(ERROR, "Memory allocation for extension failed.")
        free(file->data);
        free(file);
        return NULL;
    }

    size_t ext_pos = 0;
    unsigned char ext_char;
    do {
        ext_char = 0;
        for (int bit = 0; bit < 8; bit++) {
            ext_char = (ext_char << 1) | (bitmap->data[extension_offset + ext_pos * 8 + bit] & 0x01);
        }
        extension[ext_pos++] = ext_char;
    } while (ext_char != '\0' && ext_pos < 6);

    extension[ext_pos] = '\0';  // Ensure null-termination

    LOG(INFO, "Extracted file extension: %s", extension)

    file->extension = extension;

    return file;
}

FilePackage* extract_LSB4(const BMPImage *bitmap) {

    FilePackage *file = malloc(sizeof(FilePackage));
    if (!file) {
        LOG(ERROR, "Memory allocation failed.")
        return NULL;
    }

    // Extract the size of the file (first 32 bits)
    file->size = 0;
    for (size_t i = 0; i < 8; i++) {
        file->size = (file->size << 4) | (bitmap->data[i] & 0x0F);
    }
    LOG(INFO, "Extracted file size: %zu bytes", file->size)

    // Allocate memory for the file data
    file->data = malloc(file->size);
    if (!file->data) {
        LOG(ERROR, "Memory allocation for file data failed.")
        free(file);
        return NULL;
    }

    // Extract the file data from the bitmap using LSB4 algorithm
    size_t data_offset = 8;  // We've already processed 8 bytes (32 bits) for the file size
    for (size_t i = 0; i < file->size; i++) {
        unsigned char byte = 0;
        for (int nibble = 0; nibble < 2; nibble++) {  // Each byte is composed of two 4-bit nibbles
            byte = (byte << 4) | (bitmap->data[data_offset] & 0x0F);
            data_offset++;
        }
        file->data[i] = byte;
    }

    LOG(INFO, "File data extracted.")

    // Extract the file extension (after the file data, until the null terminator '\0')
    char *extension = malloc(6);  // Assuming max 6 characters for the extension, including '.\0'
    if (!extension) {
        LOG(ERROR, "Memory allocation for extension failed.")
        free(file->data);
        free(file);
        return NULL;
    }

    size_t ext_pos = 0;
    unsigned char ext_char;
    do {
        ext_char = 0;
        for (int nibble = 0; nibble < 2; nibble++) {  // Each character in the extension is extracted from two nibbles (4 bits)
            ext_char = (ext_char << 4) | (bitmap->data[data_offset] & 0x0F);
            data_offset++;
        }
        extension[ext_pos++] = ext_char;
    } while (ext_char != '\0' && ext_pos < 6);

    extension[ext_pos] = '\0';  // Ensure null-termination

    LOG(INFO, "Extracted file extension: %s", extension)

    // Optionally, store the extension in the FilePackage or return it as needed.
    file->extension = extension;  // Assuming the FilePackage structure has an 'extension' field

    return file;
}


// FilePackage* extract_LSBI(const BMPImage *bitmap) {
//
//     FilePackage *file = malloc(sizeof(FilePackage));
//     if (!file) {
//         LOG(ERROR, "Memory allocation failed for file package.");
//         return NULL;
//     }
//
//     // Extract the 4-bit change patterns
//     uint8_t pattern = 0;
//     for (size_t i = 0; i < 4; i++) {
//         pattern = (pattern << 1) | (bitmap->data[i] & 0x01);
//     }
//     LOG(INFO, "Extracted pattern: %u", pattern);
//
//     // Extract the file size (next 32 bits)
//     uint32_t file_size = 0;
//     for (size_t i = 4; i < 36; i++) {
//         file_size = (file_size << 1) | (bitmap->data[i] & 0x01);
//     }
//     file->size = file_size;
//     file->data = malloc(file_size);
//     if (!file->data) {
//         LOG(ERROR, "Memory allocation failed for file data.");
//         free(file);
//         return NULL;
//     }
//     LOG(INFO, "Extracted file size: %u bytes", file_size);
//
//     // Extract the file data (after the size)
//     size_t data_start = 36;
//     for (size_t i = 0; i < file_size; i++) {
//         unsigned char byte = 0;
//         for (int bit = 0; bit < 8; bit++) {
//             byte = (byte << 1) | (bitmap->data[data_start + i * 8 + bit] & 0x01);
//         }
//         file->data[i] = byte;
//     }
//     LOG(INFO, "File data extracted.");
//
//     // Extract the file extension (until the null terminator '\0')
//     size_t extension_offset = data_start + file_size * 8;
//     char extension[16] = {0};  // Assuming max 15 characters for extension
//     for (size_t i = 0; i < 15; i++) {
//         unsigned char ext_char = 0;
//         for (int bit = 0; bit < 8; bit++) {
//             ext_char = (ext_char << 1) | (bitmap->data[extension_offset + i * 8 + bit] & 0x01);
//         }
//         extension[i] = ext_char;
//         if (ext_char == '\0') break;
//     }
//     file->extension = strdup(extension);
//     LOG(INFO, "Extracted file extension: %s", file->extension);
//
//     return file;
// }

FilePackage* extract_LSBI(const BMPImage *bitmap) {
    FilePackage *file = malloc(sizeof(FilePackage));
    if (!file) {
        LOG(ERROR, "Memory allocation failed for file package.")
        return NULL;
    }

    // Extraer el patrón de inversión (4 bits)
    uint8_t pattern[2] = {0};  // El patrón es de 4 bits
    for (size_t i = 0; i < 4; i++) {
        uint8_t bit_value = bitmap->data[i] & 0x01;
        pattern[i / 2] = (pattern[i / 2] << 1) | bit_value;
    }
    LOG(INFO, "Extracted pattern: %u %u", pattern[0], pattern[1])

    // Extraer el tamaño del archivo (32 bits), teniendo en cuenta el patrón
    uint32_t file_size = 0;
    for (size_t i = 4; i < 36; i++) {
        uint8_t bit = bitmap->data[i] & 0x01;

        // Aplicar inversión si el patrón lo indica
        uint8_t pattern_bit_index = (i - 4) % 4;
        if (pattern[pattern_bit_index / 2] & (1 << (1 - pattern_bit_index % 2))) {
            bit = !bit;
        }

        file_size = (file_size << 1) | bit;
    }
    file->size = file_size;
    LOG(INFO, "Extracted file size: %u bytes", file_size)

    // Validación del tamaño
    if (file_size > bitmap->data_size / 8) {
        LOG(ERROR, "Extracted file size is larger than the available data.")
        free(file);
        return NULL;
    }

    // Asignar memoria para los datos del archivo
    file->data = malloc(file_size);
    if (!file->data) {
        LOG(ERROR, "Memory allocation failed for file data.")
        free(file);
        return NULL;
    }

    // Extraer los datos del archivo (bits embebidos con LSBI)
    size_t data_start = 36;
    for (size_t i = 0; i < file_size; i++) {
        unsigned char byte = 0;
        for (int bit = 0; bit < 8; bit++) {
            uint8_t bit_value = bitmap->data[data_start + i * 8 + bit] & 0x01;

            // Aplicar inversión si el patrón lo indica
            uint8_t pattern_bit_index = (data_start + i * 8 + bit) % 4;
            if (pattern[pattern_bit_index / 2] & (1 << (1 - pattern_bit_index % 2))) {
                bit_value = !bit_value;
            }

            byte = (byte << 1) | bit_value;
        }
        file->data[i] = byte;
    }
    LOG(INFO, "File data extracted.")

    // Extraer la extensión del archivo (máximo 15 caracteres), teniendo en cuenta el patrón
    size_t extension_offset = data_start + file_size * 8;
    char extension[16] = {0};  // Asumiendo una extensión de máximo 15 caracteres
    for (size_t i = 0; i < 15; i++) {
        unsigned char ext_char = 0;
        for (int bit = 0; bit < 8; bit++) {
            uint8_t bit_value = bitmap->data[extension_offset + i * 8 + bit] & 0x01;

            // Aplicar inversión si el patrón lo indica
            uint8_t pattern_bit_index = (extension_offset + i * 8 + bit) % 4;
            if (pattern[pattern_bit_index / 2] & (1 << (1 - pattern_bit_index % 2))) {
                bit_value = !bit_value;
            }

            ext_char = (ext_char << 1) | bit_value;
        }
        extension[i] = ext_char;
        if (ext_char == '\0') break;
    }
    file->extension = strdup(extension);
    LOG(INFO, "Extracted file extension: %s", file->extension)

    return file;
}

