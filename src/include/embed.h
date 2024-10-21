#ifndef EMBED_H
#define EMBED_H

#include <stdio.h>
#include <stdlib.h>
#include "file_handler.h"
#include "arguments.h"
#include "embed.h"
#include "logger.h"
#include "types.h"

void embed(FilePackage *bitmap, const char *input_bmp_file, const char *output_file, StegAlgorithm steg_algorithm);

#endif //EMBED_H