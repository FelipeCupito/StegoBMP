#include "./include/bmp_image.h"

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


BMPImage *new_bmp_file(const char *file_path) {
    // Check if the file path is valid
    if (file_path == NULL) {
        LOG(ERROR, "Invalid file path.")
        return NULL;
    }

    // Open the BMP file
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        LOG(ERROR, "Could not open BMP file %s.", file_path)
        return NULL;
    }

    // Allocate memory for the BMPImage structure
    BMPImage *bmp = (BMPImage *)malloc(sizeof(BMPImage));
    if (bmp == NULL) {
        LOG(ERROR, "Could not allocate memory for BMPImage.")
        fclose(file);
        return NULL;
    }

    // Read the BMP header (54 bytes)
    if (fread(bmp->header, sizeof(unsigned char), BMP_HEADER_SIZE, file) != BMP_HEADER_SIZE) {
        LOG(ERROR, "Could not read BMP header.")
        fclose(file);
        free(bmp);
        return NULL;
    }

    // Check the BMP signature ("BM")
    if (strncmp((char *)bmp->header + BMP_SIGNATURE_OFFSET, BMP_SIGNATURE, BMP_SIGNATURE_SIZE) != 0) {
        LOG(ERROR, "Invalid BMP file signature.")
        fclose(file);
        free(bmp);
        return NULL;
    }

    // Verify the size of the DIB header is 40 bytes (V3 only)
    int dib_header_size = *(int *)&bmp->header[BMP_DIB_HEADER_SIZE_OFFSET];
    if (dib_header_size != BMP_DIB_HEADER_SIZE_V3) {
        LOG(ERROR, "Unsupported BMP header size: %d bytes. Only BMP V3 with 40-byte DIB headers are supported.", dib_header_size)
        fclose(file);
        free(bmp);
        return NULL;
    }

    // Check if the image is 24 bits per pixel
    short bits_per_pixel = *(short *)&bmp->header[BMP_BITS_PER_PIXEL_OFFSET];
    if (bits_per_pixel != BMP_24_BITS) {
        LOG(ERROR, "Unsupported BMP format: Only 24-bit BMP files are supported. Found %d bits per pixel.", bits_per_pixel)
        fclose(file);
        free(bmp);
        return NULL;
    }

    // Check if the image has no compression
    int compression = *(int *)&bmp->header[BMP_COMPRESSION_OFFSET];
    if (compression != BMP_COMPRESSION_NONE) {
        LOG(ERROR, "Unsupported BMP format: Compression is not supported. Compression type found: %d.", compression)
        fclose(file);
        free(bmp);
        return NULL;
    }

    // Read the width and height of the image from the BMP header
    bmp->width = *(int *)&bmp->header[BMP_WIDTH_OFFSET];
    bmp->height = *(int *)&bmp->header[BMP_HEIGHT_OFFSET];
    if (bmp->width <= 0 || bmp->height <= 0) {
        LOG(ERROR, "Invalid BMP dimensions: width = %lu, height = %lu.", bmp->width, bmp->height)
        fclose(file);
        free(bmp);
        return NULL;
    }
    LOG(INFO, "BMP dimensions: width = %lu, height = %lu.", bmp->width, bmp->height)

    // Read the size of the pixel data from the BMP header
    bmp->data_size = *(int *)&bmp->header[BMP_IMAGE_SIZE_OFFSET];
    if (bmp->data_size <= 0) {
        LOG(ERROR, "Invalid BMP data size: %lu bytes.", bmp->data_size)
        fclose(file);
        free(bmp);
        return NULL;
    }
    LOG(INFO, "BMP data size: %lu bytes.", bmp->data_size)

    // Allocate memory for the pixel data
    bmp->data = (unsigned char *)malloc(bmp->data_size);
    if (bmp->data == NULL) {
        LOG(ERROR, "Could not allocate memory for BMP data.")
        fclose(file);
        free(bmp);
        return NULL;
    }

    // Read the pixel data from the BMP file
    if (fread(bmp->data, sizeof(unsigned char), bmp->data_size, file) != bmp->data_size) {
        LOG(ERROR, "Could not read BMP pixel data.")
        fclose(file);
        free(bmp->data);
        free(bmp);
        return NULL;
    }

    fclose(file);
    LOG(INFO, "BMP file read successfully: %s.", file_path)
    return bmp;
}

int save_bmp_file(const char *output_file, BMPImage *bmp) {

    if (output_file == NULL || bmp == NULL) {
        LOG(ERROR, "Invalid output file path or BMP image.")
        return -1;
    }

    FILE *file = fopen(output_file, "wb");
    if (file == NULL) {
        LOG(ERROR, "Could not open output file %s.", output_file)
        return -1;
    }

    // Write the BMP header
    if (fwrite(bmp->header, sizeof(unsigned char), BMP_HEADER_SIZE, file) != BMP_HEADER_SIZE) {
        LOG(ERROR, "Could not write BMP header to file %s.", output_file)
        fclose(file);
        return -1;
    }

    // Write the pixel data
    if (fwrite(bmp->data, sizeof(unsigned char), bmp->data_size, file) != bmp->data_size) {
        LOG(ERROR, "Could not write BMP pixel data to file %s.", output_file)
        fclose(file);
        return -1;
    }

    fclose(file);
    LOG(INFO, "BMP file saved successfully to %s.", output_file)
    return 0;
}


void free_bmp(BMPImage *bmp) {
    if (bmp != NULL) {
        if (bmp->data != NULL) {
            free(bmp->data);
            bmp->data = NULL;
        }
        free(bmp);
        LOG(DEBUG, "BMPImage memory freed.")
    } else {
        LOG(ERROR, "Attempted to free a NULL BMPImage pointer.")
    }
}

BMPImage* copy_bmp(BMPImage *bmp) {
    if (bmp == NULL) {
        LOG(ERROR, "copy_bmp recibió un puntero NULL.")
        return NULL;
    }

    // Asignar memoria para el nuevo BMPImage
    BMPImage* new_bmp = malloc(sizeof(BMPImage));
    if (new_bmp == NULL) {
        LOG(ERROR, "No se pudo asignar memoria para el nuevo BMPImage.")
        return NULL;
    }

    // Copiar el encabezado
    memcpy(new_bmp->header, bmp->header, BMP_HEADER_SIZE);

    // Copiar otros campos
    new_bmp->width = bmp->width;
    new_bmp->height = bmp->height;
    new_bmp->data_size = bmp->data_size;

    // Asignar y copiar los datos de píxeles
    new_bmp->data = malloc(bmp->data_size);
    if (new_bmp->data == NULL) {
        LOG(ERROR, "No se pudo asignar memoria para los datos de BMP.")
        free(new_bmp);
        return NULL;
    }
    memcpy(new_bmp->data, bmp->data, bmp->data_size);

    return new_bmp;
}

uint8_t* get_component_by_index(const BMPImage *bmp, size_t index) {
    if (bmp == NULL || bmp->data == NULL) {
        LOG(ERROR, "BMPImage o datos BMP NULL en get_component_by_index.");
        return NULL;
    }

    size_t row_size = (bmp->width * 3 + 3) & ~3;  // Ajuste para padding (tamaño real de la fila)
    size_t total_components = bmp->width * bmp->height * 3;

    if (index >= total_components) {
        LOG(ERROR, "Índice fuera de límites en get_component_by_index. Índice: %zu, Total componentes: %zu", index, total_components);
        return NULL;
    }

    size_t pixel_row = index / (bmp->width * 3);        // Fila del componente
    size_t offset_in_row = index % (bmp->width * 3);    // Desplazamiento en la fila

    // Retornar un puntero al componente en bmp_data
    return &bmp->data[pixel_row * row_size + offset_in_row];
}

//uint8_t* get_component_by_index(BMPImage *bmp, size_t index) {
//    size_t components_per_row = bmp->width * 3;
//    size_t row_size = (components_per_row + 3) & ~3;  // Tamaño real de la fila, incluyendo padding
//    size_t row = index / components_per_row;
//    size_t column = index % components_per_row;
//
//    size_t data_index = row * row_size + column;
//
//    // Verificación de límites para evitar desbordamientos
//    if (data_index >= bmp->data_size) {
//        LOG(ERROR, "Índice fuera de los límites de la imagen BMP.");
//        return NULL;
//    }
//
//    return &bmp->data[data_index];
//}


Pixel* get_pixel_by_index(const BMPImage *bmp, size_t pixel_index) {
    if (bmp == NULL || bmp->data == NULL) {
        LOG(ERROR, "BMPImage o datos BMP NULL en get_pixel_by_index.");
        return NULL;
    }

    size_t total_pixels = bmp->width * bmp->height;
    if (pixel_index >= total_pixels) {
        LOG(ERROR, "Índice de píxel fuera de límites en get_pixel_by_index. Índice: %zu, Total píxeles: %zu", pixel_index, total_pixels);
        return NULL;
    }

    size_t row_size = (bmp->width * 3 + 3) & ~3;  // Ajuste para padding
    size_t pixel_row = pixel_index / bmp->width;  // Fila del píxel (bottom-up)
    size_t offset_in_row = (pixel_index % bmp->width) * 3;  // Desplazamiento del píxel en la fila

    size_t data_offset = pixel_row * row_size + offset_in_row;
    if (data_offset + 3 > bmp->data_size) {
        LOG(ERROR, "Offset de píxel fuera de límites en get_pixel_by_index. Offset: %zu, Data size: %zu", data_offset, bmp->data_size);
        return NULL;
    }

    return (Pixel*)(&bmp->data[data_offset]);
}



//Pixel* get_pixel_by_index(BMPImage *bmp, size_t pixel_index) {
//    // Calcular el tamaño real de la fila, sin incluir padding en caso de ser innecesario
//    size_t row_size = ((bmp->width * 3) % 4 == 0) ? (bmp->width * 3) : ((bmp->width * 3 + 3) & ~3);
//
//    size_t pixel_row = pixel_index / bmp->width;          // Fila del píxel
//    size_t offset_in_row = (pixel_index % bmp->width) * 3;  // Desplazamiento del píxel en la fila
//
//    // Retornar un puntero al píxel en bmp_data
//    return (Pixel*)(&bmp->data[pixel_row * row_size + offset_in_row]);
//}



