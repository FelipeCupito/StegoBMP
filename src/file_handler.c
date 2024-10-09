#include "./include/file_handler.h"

#define BMP_SIGNATURE_OFFSET 0          // Offset for BMP signature ("BM")
#define BMP_SIGNATURE_SIZE 2            // Size of the BMP signature
#define BMP_BITS_PER_PIXEL_OFFSET 28    // Offset for the "bits per pixel" field
#define BMP_COMPRESSION_OFFSET 30       // Offset for the compression field
#define BMP_WIDTH_OFFSET 18             // Offset for the image width in the header
#define BMP_HEIGHT_OFFSET 22            // Offset for the image height in the header
#define BMP_IMAGE_SIZE_OFFSET 34        // Offset for the size of the image data (in bytes)
#define BMP_SIGNATURE "BM"              // Expected BMP signature for valid BMP files
#define BMP_24_BITS 24                  // BMP should be 24 bits per pixel
#define BMP_COMPRESSION_NONE 0          // BMP should have no compression
#define BMP_DIB_HEADER_SIZE_OFFSET 14   // Offset for the DIB header size field
#define BMP_DIB_HEADER_SIZE_V3 40       // DIB header size for V3 format

BMPImage *read_bmp_file(const char *file_path) {
    // Check if the file path is valid
    if (file_path == NULL) {
        LOG(ERROR, "Invalid file path.");
        return NULL;
    }

    // Open the BMP file
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        LOG(ERROR, "Could not open BMP file %s.", file_path);
        return NULL;
    }

    // Allocate memory for the BMPImage structure
    BMPImage *bmp = (BMPImage *)malloc(sizeof(BMPImage));
    if (bmp == NULL) {
        LOG(ERROR, "Could not allocate memory for BMPImage.");
        fclose(file);
        return NULL;
    }

    // Read the BMP header (54 bytes)
    if (fread(bmp->header, sizeof(unsigned char), BMP_HEADER_SIZE, file) != BMP_HEADER_SIZE) {
        LOG(ERROR, "Could not read BMP header.");
        fclose(file);
        free(bmp);
        return NULL;
    }

    // Check the BMP signature ("BM")
    if (strncmp((char *)bmp->header + BMP_SIGNATURE_OFFSET, BMP_SIGNATURE, BMP_SIGNATURE_SIZE) != 0) {
        LOG(ERROR, "Invalid BMP file signature.");
        fclose(file);
        free(bmp);
        return NULL;
    }

    // Verify the size of the DIB header is 40 bytes (V3 only)
    int dib_header_size = *(int *)&bmp->header[BMP_DIB_HEADER_SIZE_OFFSET];
    if (dib_header_size != BMP_DIB_HEADER_SIZE_V3) {
        LOG(ERROR, "Unsupported BMP header size: %d bytes. Only BMP V3 with 40-byte DIB headers are supported.", dib_header_size);
        fclose(file);
        free(bmp);
        return NULL;
    }

    // Check if the image is 24 bits per pixel
    short bits_per_pixel = *(short *)&bmp->header[BMP_BITS_PER_PIXEL_OFFSET];
    if (bits_per_pixel != BMP_24_BITS) {
        LOG(ERROR, "Unsupported BMP format: Only 24-bit BMP files are supported. Found %d bits per pixel.", bits_per_pixel);
        fclose(file);
        free(bmp);
        return NULL;
    }

    // Check if the image has no compression
    int compression = *(int *)&bmp->header[BMP_COMPRESSION_OFFSET];
    if (compression != BMP_COMPRESSION_NONE) {
        LOG(ERROR, "Unsupported BMP format: Compression is not supported. Compression type found: %d.", compression);
        fclose(file);
        free(bmp);
        return NULL;
    }

    // Read the width and height of the image from the BMP header
    bmp->width = *(int *)&bmp->header[BMP_WIDTH_OFFSET];
    bmp->height = *(int *)&bmp->header[BMP_HEIGHT_OFFSET];
    if (bmp->width <= 0 || bmp->height <= 0) {
        LOG(ERROR, "Invalid BMP dimensions: width = %lu, height = %lu.", bmp->width, bmp->height);
        fclose(file);
        free(bmp);
        return NULL;
    }
    LOG(INFO, "BMP dimensions: width = %lu, height = %lu.", bmp->width, bmp->height)

    // Read the size of the pixel data from the BMP header
    bmp->data_size = *(int *)&bmp->header[BMP_IMAGE_SIZE_OFFSET];
    if (bmp->data_size <= 0) {
        LOG(ERROR, "Invalid BMP data size: %lu bytes.", bmp->data_size);
        fclose(file);
        free(bmp);
        return NULL;
    }
    LOG(INFO, "BMP data size: %lu bytes.", bmp->data_size)

    // Allocate memory for the pixel data
    bmp->data = (unsigned char *)malloc(bmp->data_size);
    if (bmp->data == NULL) {
        LOG(ERROR, "Could not allocate memory for BMP data.");
        fclose(file);
        free(bmp);
        return NULL;
    }

    // Read the pixel data from the BMP file
    if (fread(bmp->data, sizeof(unsigned char), bmp->data_size, file) != bmp->data_size) {
        LOG(ERROR, "Could not read BMP pixel data.");
        fclose(file);
        free(bmp->data);
        free(bmp);
        return NULL;
    }

    fclose(file);
    LOG(INFO, "BMP file read successfully: %s.", file_path);
    return bmp;
}

int save_bmp_file(const char *output_file, BMPImage *bmp) {
    if (output_file == NULL || bmp == NULL) {
        LOG(ERROR, "Invalid output file path or BMP image.");
        return -1;
    }

    FILE *file = fopen(output_file, "wb");
    if (file == NULL) {
        LOG(ERROR, "Could not open output file %s.", output_file);
        return -1;
    }

    // Write the BMP header
    if (fwrite(bmp->header, sizeof(unsigned char), BMP_HEADER_SIZE, file) != BMP_HEADER_SIZE) {
        LOG(ERROR, "Could not write BMP header to file %s.", output_file);
        fclose(file);
        return -1;
    }

    // Write the pixel data
    if (fwrite(bmp->data, sizeof(unsigned char), bmp->data_size, file) != bmp->data_size) {
        LOG(ERROR, "Could not write BMP pixel data to file %s.", output_file);
        fclose(file);
        return -1;
    }

    fclose(file);
    LOG(INFO, "BMP file saved successfully to %s.", output_file);
    return 0;
}


void free_bmp(BMPImage *bmp) {
    if (bmp != NULL) {
        if (bmp->data != NULL) {
            free(bmp->data);
            bmp->data = NULL;
        }
        free(bmp);
        LOG(DEBUG, "BMPImage memory freed.");
    } else {
        LOG(ERROR, "Attempted to free a NULL BMPImage pointer.");
    }
}

FilePackage *create_file_package(const char *file_path){
    if (file_path == NULL) {
        LOG(ERROR, "Invalid file path.")
        return NULL;
    }

    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        LOG(ERROR, "Could not open file %s.", file_path)
        return NULL;
    }

    // Get file size
    size_t file_size = get_file_size(file);
    if (file_size == 0) {
        LOG(ERROR, "Could not get the file size.")
        fclose(file);
        return NULL;
    }

    // Allocate memory for the file data
    unsigned char *bitmap = (unsigned char *)malloc(file_size);
    if (bitmap == NULL) {
        LOG(ERROR, "Could not allocate memory for file data.")
        fclose(file);
        return NULL;
    }

    // Read the file data
    if (fread(bitmap, 1, file_size, file) != file_size){
        LOG(ERROR, "Could not read the file data.")
        fclose(file);
        free(bitmap);
        return NULL;
    }

    // Get the file extension
    char *extension = get_file_extension(file_path);
    if (extension == NULL) {
        LOG(ERROR, "Could not get the file extension.")
        free(bitmap);
        fclose(file);
        return NULL;
    }

    // Create the FilePackage structure
    FilePackage *package = (FilePackage *)malloc(sizeof(FilePackage));
    if (package == NULL) {
        LOG(ERROR, "Could not allocate memory for the FilePackage.")
        free(bitmap);
        free(extension);
        fclose(file);
        return NULL;
    }

    // Fill in the FilePackage structure
    package->size = file_size;
    package->data = bitmap;
    package->extension = extension;

    fclose(file);  // Close the file after reading

    LOG(INFO, "File package created successfully: size = %lu, extension = %s", file_size, extension)
    return package;
}


int create_file_from_package(const char *filename, FilePackage *package) {
    //TODO: falta testear
    if(filename == NULL){
        LOG(ERROR, "Invalid filename.")
        return -1;
    }

    if ( package == NULL || package->extension == NULL || package->data == NULL) {
        LOG(ERROR, "Invalid FilePackage pointer or data.");
        return -1;
    }

    // Concatenate filename and extension to form the full filename
    size_t full_filename_length = strlen(filename) + strlen(package->extension) + 1; // +1 for null terminator
    char *full_filename = (char *)malloc(full_filename_length);
    if (full_filename == NULL) {
        LOG(ERROR, "Could not allocate memory for the full filename.");
        return -1;
    }

    snprintf(full_filename, full_filename_length, "%s%s", filename, package->extension);

    // Open the file for writing
    FILE *file = fopen(full_filename, "wb");
    if (file == NULL) {
        LOG(ERROR, "Could not open file %s for writing.", full_filename);
        free(full_filename);
        return -1;
    }

    // Write the file data from the FilePackage
    size_t written_bytes = fwrite(package->data, 1, package->size, file);
    if (written_bytes != package->size) {
        LOG(ERROR, "Error writing data to file %s. Expected %lu bytes, wrote %lu bytes.",
                    full_filename, package->size, written_bytes);
        fclose(file);
        free(full_filename);
        return -1;
    }

    // Successfully written the file
    LOG(INFO, "Successfully created file %s with size %lu bytes.", full_filename, package->size);

    // Clean up
    fclose(file);
    free(full_filename);

    return 0;
}

void free_file_package(FilePackage *package) {
    if (package) {
        free(package->data);
        free(package->extension);
        free(package);
        LOG(DEBUG, "File package memory freed.")
    }else{
        LOG(ERROR, "Invalid FilePackage pointer.")
    }
}

void print_file_package(FilePackage *package) {
    if (package) {
        printf("File size: %lu bytes\n", package->size);
        printf("File extension: %s\n", package->extension);

        // Print the first 10 bytes of the file data for inspection
        printf("First bytes of the file data: ");
        for (int i = 0; i < 10 && i < package->size; i++) {
            printf("%02X ", package->data[i]);
        }
        printf("\n");
        LOG(INFO, "Printed file package: size = %lu, extension = %s", package->size, package->extension)
    } else {
        LOG(ERROR, "Invalid FilePackage pointer.")
    }
}


/****************************************************
 ***       Private functions implementation      ***
 ****************************************************/

/**
 * Get the size of a file.
 * @param file File pointer
 * @return Size of the file or 0 if there was an error.
 */
size_t get_file_size(FILE *file){
    if (file == NULL) {
        LOG(ERROR, "Invalid file pointer.")
        return 0;
    }
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);  // Reset file pointer to the beginning

    LOG(DEBUG, "File size: %lu bytes", file_size)
    return file_size;
}

/**
 * Get the file extension from the filename.
 * @param filename Name of the file
 * @return Dynamically allocated string with the file extension, or NULL if there is no valid extension.
 *         The caller is responsible for freeing the memory.
 */
char *get_file_extension(const char *filename) {
    if (filename == NULL) {
        LOG(ERROR, "Invalid filename.")
        return NULL;
    }

    char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) {
        LOG(ERROR, "No valid file extension found.")
        return NULL;
    }

    LOG(DEBUG, "File extension found: %s.", dot)
    return strdup(dot);  // Duplicate the extension string
}
