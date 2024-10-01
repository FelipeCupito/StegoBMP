//
// Created by Felipe Cupitó on 30/09/2024.
//

#ifndef STEGOBMP_ARGUMENTS_H
#define STEGOBMP_ARGUMENTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "logger.h"
#include "types.h"

// Valores por defecto
#define DEFAULT_ENCRYPTION_ALGO ENC_AES128
#define DEFAULT_ENCRYPTION_MODE ENC_MODE_CBC
#define DEFAULT_LOG_LEVEL INFO

// Tamaño máximo para la contraseña
#define MAX_PASSWORD_LENGTH 128

// Estructura para almacenar las opciones del programa
typedef struct {
    OperationMode mode;
    const char *input_file;
    const char *input_bmp_file;
    const char *output_bmp_file;
    StegAlgorithm steg_algorithm;
    EncryptionAlgorithm encryption_algo;
    EncryptionMode encryption_mode;
    char password[MAX_PASSWORD_LENGTH];
    LogLevel log_level;
} ProgramOptions;

// Funciones para parsear los argumentos
int parse_arguments(int argc, char *argv[], ProgramOptions *options);
void print_usage(const char *program_name);

#endif //STEGOBMP_ARGUMENTS_H
