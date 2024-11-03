
#include "file_package.h"

#define EXTENSION_SIZE 16 // Maximum size of the file extensio

#define IS_BIG_ENDIAN true

size_t get_file_size(FILE *file);
uint8_t* get_file_extension(const char *file_name);

FilePackage *new_file_package(const char *file_path){
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
    LOG(DEBUG, "[Embed File] File size: %lu bytes.", file_size)

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
    uint8_t* extension = get_file_extension(file_path);
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

    LOG(INFO, "[Embed File]  File package created successfully: size = %lu, extension = %s", file_size, extension)
    return package;
}

uint8_t* embed_data_from_file(const char *file_path, size_t *buffer_size) {
    if (file_path == NULL) {
        LOG(ERROR, "Invalid file path.")
        return NULL;
    }

    size_t buffer_index = 0;

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
    LOG(DEBUG, "[Embed File] File size: %lu bytes.", file_size)

    // Allocate memory for the file data
    uint8_t *buffer = (uint8_t *)malloc(file_size + sizeof(uint32_t) + EXTENSION_SIZE);
    if (buffer == NULL) {
        LOG(ERROR, "Could not allocate memory for the buffer.")
        fclose(file);
        return NULL;
    }

    // set the size of the file
    memcpy(&buffer[buffer_index], &file_size, sizeof(uint32_t));
    buffer_index += sizeof(uint32_t);

    // Read the file data
    if (fread(&buffer[buffer_index], 1, file_size, file) != file_size){
        LOG(ERROR, "Could not read the file data.")
        fclose(file);
        free(buffer);
        return NULL;
    }
    buffer_index += file_size;
    LOG(DEBUG, "[Embed File] File data read successfully.")

    // Get the file extension
    uint8_t* extension = get_file_extension(file_path);
    if (extension == NULL) {
        LOG(ERROR, "Could not get the file extension.")
        free(buffer);
        fclose(file);
        return NULL;
    }
    size_t  extension_size = strlen((const char*)extension) + 1;
    memcpy(&buffer[buffer_index], extension, extension_size);
    buffer_index += extension_size;
    LOG(DEBUG, "[Embed File] File extension: %s.", extension)

    // Close the file and free the extension
    free(extension);
    fclose(file);

    // Set the buffer size and return the buffer
    *buffer_size = buffer_index;
    return buffer;
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
        LOG(ERROR, "Invalid file data size: %u bytes.", file->size)
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
    file->extension = (uint8_t*) strdup(extension); // strdup asigna y copia la cadena
    if (file->extension == NULL){
        LOG(ERROR, "Could not allocate memory for file extension.")
        free(file->data);
        free(file);
        return NULL;
    }

    LOG(INFO, "FilePackage created successfully: size = %u, extension = %s", file->size, file->extension)
    return file;
}


uint8_t* create_data_buffer(const FilePackage *package, size_t *buffer_size) {
    if (package == NULL || package->data == NULL || package->extension == NULL) {
        LOG(ERROR, "Argumentos inválidos en create_data_buffer.")
        return NULL;
    }

    // Tamaño del campo de tamaño (4 bytes) + datos + extensión + byte nulo para la extensión
    size_t extension_length = strlen((char*)package->extension) + 1;  // +1 para incluir el '\0'
    *buffer_size = sizeof(package->size) + package->size + extension_length;

    uint8_t *buffer = (uint8_t *)malloc(*buffer_size);
    if (buffer == NULL) {
        LOG(ERROR, "No se pudo asignar memoria para el buffer de datos.")
        return NULL;
    }

    size_t offset = 0;

    // Convertir el tamaño a big-endian o little-endian según sea necesario
    uint32_t data_size = package->size;
    if(IS_BIG_ENDIAN){
        buffer[offset++] = (data_size >> 24) & 0xFF;
        buffer[offset++] = (data_size >> 16) & 0xFF;
        buffer[offset++] = (data_size >> 8) & 0xFF;
        buffer[offset++] = data_size & 0xFF;
    } else {
        buffer[offset++] = data_size & 0xFF;
        buffer[offset++] = (data_size >> 8) & 0xFF;
        buffer[offset++] = (data_size >> 16) & 0xFF;
        buffer[offset++] = (data_size >> 24) & 0xFF;
    }

    // Copiar los datos
    memcpy(buffer + offset, package->data, package->size);
    offset += package->size;

    // Copiar la extensión incluyendo el '\0'
    memcpy(buffer + offset, package->extension, extension_length);
    offset += extension_length;

    if (offset != *buffer_size) {
        LOG(ERROR, "Error al crear el buffer de datos: tamaños inconsistentes.")
        free(buffer);
        return NULL;
    }

    return buffer;
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

    if (strlen((char*)package->extension) >= EXTENSION_SIZE){
        LOG(ERROR, "File extension too long.");
        return -1;
    }

    // Concatenate filename and extension to form the full filename
    size_t full_filename_length = strlen(filename) + strlen((char*)package->extension) + 1; // +1 for null terminator
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
        LOG(ERROR, "Error writing data to file %s. Expected %u bytes, wrote %lu bytes.",
            full_filename, package->size, written_bytes)
        fclose(file);
        free(full_filename);
        return -1;
    }

    // Successfully written the file
    LOG(INFO, "Successfully created file %s with size %u bytes.", full_filename, package->size)

    // Clean up
    fclose(file);
    free(full_filename);

    return 1;
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
    if (package == NULL) return;

    if (package->data != NULL) {
        free(package->data);
    }

    if (package->extension != NULL) {
        free(package->extension);
    }

    free(package);
}

void print_file_package(FilePackage *package) {
    if (package) {
        printf("File size: %u bytes\n", package->size);
        printf("File extension: %s\n", package->extension);

        // Print the first 10 bytes of the file data for inspection
        printf("First bytes of the file data: ");
        for (int i = 0; i < 10 && i < package->size; i++) {
            printf("%02X ", package->data[i]);
        }
        printf("\n");
        LOG(INFO, "Printed file package: size = %u, extension = %s", package->size, package->extension)
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
 * @param file_name Name of the file
 * @return Dynamically allocated string with the file extension, or NULL if there is no valid extension.
 *         The caller is responsible for freeing the memory.
 */
uint8_t * get_file_extension(const char *file_name) {
    if (file_name == NULL) {
        LOG(ERROR, "Invalid filename.");
        return NULL;
    }

    char *dot = strrchr(file_name, '.');
    if (!dot || dot == file_name) {
        LOG(ERROR, "No valid file extension found.");
        return NULL;
    }

    size_t ext_length = strlen(dot);
    if (ext_length >= EXTENSION_SIZE) {
        LOG(ERROR, "File extension too long.");
        return NULL;
    }

    LOG(DEBUG, "File extension found: %s.", dot);
    return (uint8_t*) strdup(dot);
}

