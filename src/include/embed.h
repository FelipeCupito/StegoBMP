#ifndef EMBED_H
#define EMBED_H

#include <stdio.h>
#include <stdlib.h>
#include "./include/file_handler.h"
#include "./include/arguments.h"
#include ".include/embed.h"
#include ".include/logger.h"
#include ".include/types.h"

void embed(FilePackage *bitmap, const char *input_bmp_file, const char *output_file, StegAlgorithm steg_algorithm);

#endif //EMBED_H