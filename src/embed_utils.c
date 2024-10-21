#include "./include/embed_utils.h"

void embed_LSB1(FilePackage *bitmap, FilePackage *file) {
    // Check if the bitmap is NULL
    if (bitmap == NULL) {
        LOG(ERROR, "Invalid bitmap.")
        return;
    }

    // Check if the file is NULL
    if (file == NULL) {
        LOG(ERROR, "Invalid file.")
        return;
    }

    // Check if the file is too large to embed in the bitmap
    if (file->size > bitmap->size) {
        LOG(ERROR, "File is too large to embed in the bitmap.")
        return;
    }

    // Embed the file data into the bitmap using the LSB1 algorithm
    for (size_t i = 0; i < file->size; i++) {
        // Get the current byte from the file data
        unsigned char byte = file->data[i];

        // Embed the byte into the bitmap
        bitmap->data[i] = (bitmap->data[i] & 0xFE) | ((byte >> 7) & 0x01);
    }

    LOG(INFO, "File embedded using LSB1 algorithm.")

}

void embed_LSB4(FilePackage *bitmap, FilePackage *file) {
    // Check if the bitmap is NULL
    if (bitmap == NULL) {
        LOG(ERROR, "Invalid bitmap.")
        return;
    }

    // Check if the file is NULL
    if (file == NULL) {
        LOG(ERROR, "Invalid file.")
        return;
    }

    // Check if the file is too large to embed in the bitmap
    if (file->size * 2 > bitmap->size) {
        LOG(ERROR, "File is too large to embed in the bitmap.")
        return;
    }

    // Embed the file data into the bitmap using the LSB4 algorithm
    for (size_t i = 0; i < file->size; i++) {
        // Get the current byte from the file data
        unsigned char byte = file->data[i];

        // Embed the byte into the bitmap
        bitmap->data[i * 2] = (bitmap->data[i * 2] & 0xF0) | ((byte >> 4) & 0x0F);
        bitmap->data[i * 2 + 1] = (bitmap->data[i * 2 + 1] & 0xF0) | (byte & 0x0F);
    }

    LOG(INFO, "File embedded using LSB4 algorithm.")
}

void embed_LSBI(FilePackage *bitmap, FilePackage *file) {
    // Check if the bitmap is NULL
    if (bitmap == NULL) {
        LOG(ERROR, "Invalid bitmap.")
        return;
    }

    // Check if the file is NULL
    if (file == NULL) {
        LOG(ERROR, "Invalid file.")
        return;
    }

    // Check if the file is too large to embed in the bitmap
    if (file->size * 8 > bitmap->size) {
        LOG(ERROR, "File is too large to embed in the bitmap.")
        return;
    }

    // Embed the file data into the bitmap using the LSBI algorithm
    for (size_t i = 0; i < file->size; i++) {
        // Get the current byte from the file data
        unsigned char byte = file->data[i];

        // Embed the byte into the bitmap
        for (int j = 0; j < 8; j++) {
            bitmap->data[i * 8 + j] = (bitmap->data[i * 8 + j] & 0xFE) | ((byte >> (7 - j)) & 0x01);
        }
    }

    LOG(INFO, "File embedded using LSBI algorithm.")
}