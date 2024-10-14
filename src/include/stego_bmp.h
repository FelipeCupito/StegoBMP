#ifndef EMBED_H
#define EMBED_H

#include <stdio.h>
#include <stdlib.h>
#include "file_handler.h"
#include "logger.h"
#include "types.h"

BMPImage* embed( BMPImage *bitmap, FilePackage *message, StegAlgorithm steg_algorithm);


#ifdef TESTING
/**
 * Only used for testing purposes.
 */
void embed_LSB1(BMPImage *bitmap, FilePackage *file);
void embed_LSB4(BMPImage *bitmap, FilePackage *file);
void embed_LSBI(BMPImage *bitmap, FilePackage *file);
#endif

#endif //EMBED_H