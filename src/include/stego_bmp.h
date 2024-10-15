#ifndef EMBED_H
#define EMBED_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "file_handler.h"
#include "logger.h"
#include "types.h"

BMPImage* embed( BMPImage *bitmap, FilePackage *message, StegAlgorithm steg_algorithm);
FilePackage* extract(BMPImage* bmp_embed, StegAlgorithm steg_algorithm);


#ifdef TESTING
/**
 * Only used for testing purposes.
 */
void embed_LSB1(const BMPImage *bitmap, const FilePackage *file);
void embed_LSB4(const BMPImage *bitmap, const FilePackage *file);
void embed_LSBI(const BMPImage *bitmap, const FilePackage *file, const uint8_t *pattern);
FilePackage* extract_LSB1(const BMPImage *bitmap);
FilePackage* extract_LSB4(const BMPImage *bitmap);
FilePackage* extract_LSBI(const BMPImage *bitmap);
#endif

#endif //EMBED_H