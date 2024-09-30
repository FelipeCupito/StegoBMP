//
// Created by Felipe Cupitó on 30/09/2024.
//

#include "./include/file_handler.h"

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

/**
 * Create a FilePackage by reading file data into memory.
 * @param file_path Path to the file
 * @return Pointer to the dynamically allocated FilePackage or NULL if there was an error.
 *         The caller is responsible for freeing the FilePackage using free_file_package.
 */
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

/**
 * Free the memory associated with a FilePackage.
 * @param package Pointer to the FilePackage
 */
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

/**
 * Print the information contained in a FilePackage.
 * @param package Pointer to the FilePackage
 */
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
