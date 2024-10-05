#include "./include/embed.h"

void embed( FilePackage *bitmap, const char *input_bmp_file, const char *output_file, , StegAlgorithm steg_algorithm){

    //The possible steganography algorithms are:
    // LSB1, LSB4, LSBI
    // The embedded file will be saved in the output_file
    // The bitmap is the file to be embedded
    // The input_bmp_file is the carrier file
    // The output_file is the file where the embedded data will be saved
    // The steg_algorithm is the steganography algorithm to use
    
    // Check if the bitmap is NULL
    if (bitmap == NULL) {
        LOG(ERROR, "Invalid bitmap.")
        return;
    }

    // Check if the input_bmp_file is NULL
    if (input_bmp_file == NULL) {
        LOG(ERROR, "Invalid input BMP file.")
        return;
    }

    // Check if the output_file is NULL
    if (output_file == NULL) {
        LOG(ERROR, "Invalid output file.")
        return;
    }

    // Check if the steg_algorithm is invalid
    if (steg_algorithm == STEG_NONE) {
        LOG(ERROR, "Invalid steganography algorithm.")
        return;
    }

    // Load the BMP file
    FilePackage *bmp_package = create_file_package(input_bmp_file);
    if (bmp_package == NULL) {
        LOG(ERROR, "Error loading BMP file.")
        return;
    }

    // Check if the BMP file is valid
    if (bmp_package->data == NULL || bmp_package->size == 0) {
        LOG(ERROR, "Invalid BMP file.")
        free_file_package(bmp_package);
        return;
    }

    // Check if the bitmap is too small to embed the file
    if (bitmap->size > bmp_package->size) {
        LOG(ERROR, "Bitmap is too small to embed the file.")
        free_file_package(bmp_package);
        return;
    }

    // Embed the file into the bitmap
    switch (steg_algorithm) {
        case STEG_LSB1:
            embed_LSB1(bitmap, bmp_package);
            break;
        case STEG_LSB4:
            embed_LSB4(bitmap, bmp_package);
            break;
        case STEG_LSBI:
            embed_LSBI(bitmap, bmp_package);
            break;
        default:
            LOG(ERROR, "Invalid steganography algorithm.")
            break;
    }

    // Save the modified bitmap to the output file
    if (!create_file_from_package(output_file, bitmap)) {
        LOG(ERROR, "Error saving the output file.")
    }

    // Clean up
    free_file_package(bmp_package);
    LOG(INFO, "File embedded successfully.")

    return;
}