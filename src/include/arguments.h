#ifndef STEGOBMP_ARGUMENTS_H
#define STEGOBMP_ARGUMENTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "logger.h"
#include "types.h"

// Default values for encryption algorithm and mode, and log level
#define DEFAULT_ENCRYPTION_ALGO ENC_AES128      // Default encryption algorithm: AES128
#define DEFAULT_ENCRYPTION_MODE ENC_MODE_CBC    // Default encryption mode: CBC
#define DEFAULT_LOG_LEVEL NONE                  // Default log level: INFO

// Maximum allowed password length
#define MAX_PASSWORD_LENGTH 128

/**
 * @brief Structure to store the parsed command line options.
 *
 * This structure holds all the necessary options parsed from the command-line arguments.
 * It includes information about the operation mode (embed or extract), input/output files,
 * steganography algorithm, encryption settings, password, and logging level.
 */
typedef struct {
    OperationMode mode;                     // Operation mode: either embed or extract
    const char *input_file;                 // File to be embedded (in embed mode)
    const char *input_bmp_file;             // Input BMP file (carrier in embed mode, source in extract mode)
    const char *output_file;            // Output BMP file (for embedding) or file to save the extracted data
    StegAlgorithm steg_algorithm;           // Steganography algorithm to use (LSB1, LSB4, LSBI)
    EncryptionAlgorithm encryption_algo;    // Encryption algorithm (aes128, aes192, aes256, 3des)
    EncryptionMode encryption_mode;         // Encryption mode (ecb, cfb, ofb, cbc)
    char password[MAX_PASSWORD_LENGTH];     // Password for encryption/decryption
} ProgramOptions;

/**
 * @brief Parse the command line arguments and populate the ProgramOptions structure.
 *
 * This function takes the command-line arguments passed to the program and extracts the relevant
 * options, storing them in the provided `options` structure. It uses `getopt_long_only` to parse
 * long-form options such as `-embed`, `-in`, and `-p`.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @param options Pointer to a `ProgramOptions` structure that will store the parsed options.
 * @return 1 on success, 0 if an error occurs (invalid or missing arguments).
 */
int parse_arguments(int argc, char *argv[], ProgramOptions *options);

/**
 * @brief Log the parsed program options.
 *
 * This function logs all the options stored in the ProgramOptions structure,
 * including operation mode, files, algorithms, and encryption settings.
 *
 * @param options The ProgramOptions structure with all parsed options.
 */
void log_program_options(const ProgramOptions *options);


#ifdef TESTING
/**
 * Only used for testing purposes.
 */
void print_usage(const char *program_name);
OperationMode parse_operation_mode(const char *str);
StegAlgorithm parse_steg_algorithm(const char *str);
EncryptionAlgorithm parse_encryption_algorithm(const char *str);
EncryptionMode parse_encryption_mode(const char *str);
const char* operation_mode_to_string(OperationMode mode);
const char* steg_algorithm_to_string(StegAlgorithm alg);
const char* encryption_algorithm_to_string(EncryptionAlgorithm alg);
const char* encryption_mode_to_string(EncryptionMode mode);
#endif

#endif //STEGOBMP_ARGUMENTS_H
