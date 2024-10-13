#include "./include/embed.h"

void embed_LSB1(BMPImage *bitmap, FilePackage *file);
void embed_LSB4(BMPImage *bitmap, FilePackage *file);
void embed_LSBI(BMPImage *bitmap, FilePackage *file);

BMPImage embed(BMPImage *bitmap, FilePackage *message, StegAlgorithm steg_algorithm){

    //The possible steganography algorithms are:
    // LSB1, LSB4, LSBI
    // The embedded file will be returned as a BMPImage struct
    // The message is the file to be embedded
    // The steg_algorithm is the steganography algorithm to use

    // Select the appropriate embedding function based on the steg_algorithm
    switch (steg_algorithm) {
    case STEG_LSB1:
        embed_LSB1(bitmap, message);
        break;
    case STEG_LSB4:
        embed_LSB4(bitmap, message);
        break;
    case STEG_LSBI:
        embed_LSBI(bitmap, message);
        break;
    default:
        LOG(ERROR, "Invalid steganography algorithm.")
        break;
    }

    BMPImage embedded_image = *bitmap;

    return embedded_image;
}

void embed_LSB1(BMPImage *bitmap, FilePackage *file) {
    // Check if the file is too large to embed in the bitmap
    if (file->size * 8 > bitmap->data_size) {
        LOG(ERROR, "File is too large to embed in the bitmap.");
        return;
    }

    // Embed the file data into the bitmap using the LSB1 algorithm
    for (size_t i = 0; i < file->size; i++) {
        unsigned char byte = file->data[i];
        for (int bit = 0; bit < 8; bit++) {
            bitmap->data[i * 8 + bit] = (bitmap->data[i * 8 + bit] & 0xFE) | ((byte >> (7 - bit)) & 0x01);
        }
    }

    LOG(INFO, "File embedded using LSB1 algorithm.");
}

void embed_LSB4(BMPImage *bitmap, FilePackage *file) {
    // Check if the file is too large to embed in the bitmap
    if (file->size * 2 > bitmap->data_size) {
        LOG(ERROR, "File is too large to embed in the bitmap.");
        return;
    }

    // Embed the file data into the bitmap using the LSB4 algorithm
    for (size_t i = 0; i < file->size; i++) {
        unsigned char byte = file->data[i];
        bitmap->data[i * 2] = (bitmap->data[i * 2] & 0xF0) | ((byte >> 4) & 0x0F);
        bitmap->data[i * 2 + 1] = (bitmap->data[i * 2 + 1] & 0xF0) | (byte & 0x0F);
    }

    LOG(INFO, "File embedded using LSB4 algorithm.");
}

void embed_LSBI(BMPImage *bitmap, FilePackage *file) {
    //` Check if the file is too large to embed in the bitmap
    if (file->size * 8 > bitmap->data_size) {
        LOG(ERROR, "File is too large to embed in the bitmap.");
        return;
    }

    // Embed the file data into the bitmap using the LSBI algorithm
    for (size_t i = 0; i < file->size; i++) {
        unsigned char byte = file->data[i];
        for (int bit = 0; bit < 8; bit++) {
            bitmap->data[i * 8 + bit] = (bitmap->data[i * 8 + bit] & 0xFE) | ((byte >> (7 - bit)) & 0x01);
        }
    }

    LOG(INFO, "File embedded using LSBI algorithm.");
}