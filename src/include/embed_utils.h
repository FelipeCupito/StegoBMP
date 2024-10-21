#ifndef EMBED_UTILS_H
#define EMBED_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "file_handler.h"
#include "arguments.h"
#include "logger.h"
#include "types.h"

void embed_LSB1(FilePackage *bitmap, FilePackage *file);
void embed_LSB4(FilePackage *bitmap, FilePackage *file);
void embed_LSBI(FilePackage *bitmap, FilePackage *file);

#endif //EMBED_UTILS_H