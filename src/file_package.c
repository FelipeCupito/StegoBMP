
#include "file_package.h"

#define EXTENSION_SIZE 16               // Maximum size of the file extension


size_t get_file_size(FILE *file);
char *get_file_extension(const char *filename);

FilePackage *new_file_package(const char *filepath){
    if (filepath == NULL) {
        LOG(ERROR, "Invalid file path.")
        return NULL;
    }

    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        LOG(ERROR, "Could not open file %s.", filepath)
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
    char *extension = get_file_extension(filepath);
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


FilePackage *new_file_package_from_data(const uint8_t *data){
    if(data == NULL){
        LOG(ERROR, "Invalid data pointer.")
        return NULL;
    }

    // Asignar memoria para la estructura FilePackage
    FilePackage *file = (FilePackage *)malloc(sizeof(FilePackage));
    if (file == NULL) {
        LOG(ERROR, "Could not allocate memory for FilePackage.")
        return NULL;
    }

    // Extraer el tamaño de los datos (los primeros 4 bytes)
    file->size = *(int *)&data[0];
    if (file->size <= 0) {
        LOG(ERROR, "Invalid file data size: %lu bytes.", file->size)
        free(file);
        return NULL;
    }

    // Asignar memoria para los datos del archivo y copiarlos
    file->data = (unsigned char *)malloc(file->size);
    if (file->data == NULL) {
        LOG(ERROR, "Could not allocate memory for file data.")
        free(file);
        return NULL;
    }
    memcpy(file->data, data + sizeof(int), file->size); // Reemplazado memccpy por memcpy

    // Extraer la extensión del archivo
    char extension[EXTENSION_SIZE] = {0};
    // Leer desde data + sizeof(int) + file->size
    for (int i = 0; i < EXTENSION_SIZE - 1; i++) { // Reservar espacio para '\0'
        extension[i] = (char)data[sizeof(int) + file->size + i];
        if(extension[i] == '\0'){
            break;
        }
    }

    // Check if the extension is valid
    if(extension[0] != '.' || strlen(extension) < 2){
        LOG(ERROR, "Invalid file extension: %s.", extension)
        free(file->data);
        free(file);
        return NULL;
    }

    // Asignar memoria y copiar la extensión
    file->extension = strdup(extension); // strdup asigna y copia la cadena
    if (file->extension == NULL){
        LOG(ERROR, "Could not allocate memory for file extension.")
        free(file->data);
        free(file);
        return NULL;
    }

    LOG(INFO, "FilePackage created successfully: size = %lu, extension = %s", file->size, file->extension)
    return file;
}




int create_file_from_package(const char *filename, FilePackage *package) {
    if(filename == NULL){
        LOG(ERROR, "Invalid filename.")
        return -1;
    }

    if ( package == NULL || package->extension == NULL || package->data == NULL) {
        LOG(ERROR, "Invalid FilePackage pointer or data.")
        return -1;
    }

    // Concatenate filename and extension to form the full filename
    size_t full_filename_length = strlen(filename) + strlen(package->extension) + 1; // +1 for null terminator
    char *full_filename = (char *)malloc(full_filename_length);
    if (full_filename == NULL) {
        LOG(ERROR, "Could not allocate memory for the full filename.")
        return -1;
    }

    snprintf(full_filename, full_filename_length, "%s%s", filename, package->extension);

    // Open the file for writing
    FILE *file = fopen(full_filename, "wb");
    if (file == NULL) {
        LOG(ERROR, "Could not open file %s for writing.", full_filename)
        free(full_filename);
        return -1;
    }

    // Write the file data from the FilePackage
    size_t written_bytes = fwrite(package->data, 1, package->size, file);
    if (written_bytes != package->size) {
        LOG(ERROR, "Error writing data to file %s. Expected %lu bytes, wrote %lu bytes.",
            full_filename, package->size, written_bytes)
        fclose(file);
        free(full_filename);
        return -1;
    }

    // Successfully written the file
    LOG(INFO, "Successfully created file %s with size %lu bytes.", full_filename, package->size)

    // Clean up
    fclose(file);
    free(full_filename);

    return 0;
}

int create_file_from_raw_data(const char *filename, const uint8_t *data){

    if(filename == NULL || data == NULL){
        LOG(ERROR, "Invalid filename or data pointer.")
        return -1;
    }

    FilePackage  *file = new_file_package_from_data(data);
    if(file == NULL){
        LOG(ERROR, "Could not create FilePackage from raw data.")
        return -1;
    }

    int result = create_file_from_package(filename, file);
    free_file_package(file);
    return result;
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
