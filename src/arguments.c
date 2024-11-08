
#include "./include/arguments.h"

/**
 * Functon to parse strings to enums
 */
OperationMode parse_operation_mode(const char *str);
StegAlgorithm parse_steg_algorithm(const char *str);
EncryptionAlgorithm parse_encryption_algorithm(const char *str);
EncryptionMode parse_encryption_mode(const char *str);

/**
 * Functions to convert enums to strings
 */
const char* operation_mode_to_string(OperationMode mode);
const char* steg_algorithm_to_string(StegAlgorithm alg);
const char* encryption_algorithm_to_string(EncryptionAlgorithm alg);
const char* encryption_mode_to_string(EncryptionMode mode);

/**
 * @brief Print the usage message for the program.
 *
 * This function prints detailed usage instructions on how to use the program, including required
 * and optional command-line arguments for both embedding and extracting information using steganography.
 *
 * @param program_name The name of the program (usually argv[0]).
 */
void print_usage(const char *program_name);


/**
 * @brief Parse the command line arguments to set the log level.
 *
 * This function parses the command line arguments to find the log level option (-loglevel) and sets the log level
 * accordingly. If no log level is specified, the default log level is set. This function should be called before
 * parsing the other command line arguments to can log the parsing process.
 */
int parse_log_level_argument(int argc, char *argv[]);


int parse_arguments(int argc, char *argv[], ProgramOptions *options) {
    // Initialize options
    options->mode = MODE_NONE;
    options->input_file = NULL;
    options->input_bmp_file = NULL;
    options->output_file = NULL;
    options->steg_algorithm = STEG_NONE;
    options->encryption_algo = ENC_NONE;
    options->encryption_mode = ENC_MODE_NONE;
    options->password[0] = '\0';

    int opt;
    int option_index = 0;

    // Options definition
    static struct option long_options[] = {
            {"embed",      no_argument,       NULL,  'e' },
            {"extract",    no_argument,       NULL,  'x' },
            {"in",         required_argument, NULL,  'i' },
            {"p",          required_argument, NULL,  'p' },
            {"out",        required_argument, NULL,  'o' },
            {"steg",       required_argument, NULL,  's' },
            {"a",          required_argument, NULL,  'a' },
            {"m",          required_argument, NULL,  'm' },
            {"pass",       required_argument, NULL,  'P' },
            {"loglevel",   required_argument, NULL,  'l' },
            {NULL,            0,                 NULL,   0  }
    };

    // First parse log level argument and set log level, so we can log properly the parsing process
    if (!parse_log_level_argument(argc, argv)){
        return 0;
    }

    // Parse command line arguments
    while ((opt = getopt_long_only(argc, argv, "", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'e':
                options->mode = MODE_EMBED;
                LOG(DEBUG, "[arguments] Embed mode: %s", operation_mode_to_string(options->mode))
                break;
            case 'x':
                options->mode = MODE_EXTRACT;
                LOG(DEBUG, "[arguments] Extract mode: %s", operation_mode_to_string(options->mode))
                break;
            case 'i':
                options->input_file = optarg;
                LOG(DEBUG, "[arguments] Input file: %s", options->input_file)
                break;
            case 'p':
                options->input_bmp_file = optarg;
                LOG(DEBUG, "[arguments] Input BMP file: %s", options->input_bmp_file)
                break;
            case 'o':
                options->output_file = optarg;
                LOG(DEBUG, "[arguments] Output BMP file: %s", options->output_file)
                break;
            case 's':
                options->steg_algorithm = parse_steg_algorithm(optarg);
                LOG(DEBUG, "[arguments] Steganography algorithm: %s", optarg)
                break;
            case 'a':
                options->encryption_algo = parse_encryption_algorithm(optarg);
                LOG(DEBUG, "[arguments] Encryption algorithm: %s", optarg)
                break;
            case 'm':
                options->encryption_mode = parse_encryption_mode(optarg);
                LOG(DEBUG, "[arguments] Encryption mode: %s", optarg)
                break;
            case 'P':
                if(strlen(optarg) > MAX_PASSWORD_LENGTH) {
                    LOG(WARNING, "[arguments] Password is too long. Used only the first %d characters.", MAX_PASSWORD_LENGTH - 1)
                    return 0;
                }
                strncpy(options->password, optarg, MAX_PASSWORD_LENGTH - 1);
                options->password[MAX_PASSWORD_LENGTH - 1] = '\0';
                LOG(DEBUG, "[arguments] Password: %s", options->password)
                break;
            case 'l':
                // Skip log level argument. Already parsed.
                break;
            default:
                print_usage(argv[0]);
                return 0;
        }
    }
    LOG(DEBUG, "[arguments] Parsed command line arguments successfully.")

    // Validated required arguments
    if( options->mode == MODE_NONE ||
        options->input_bmp_file == NULL ||
        options->output_file == NULL ||
        options->steg_algorithm == STEG_NONE)
    {
        LOG(ERROR, "Missing required arguments.")
        print_usage(argv[0]);
        return 0;
    }

    // Check -in argument with embed mode
    if (options->mode == MODE_EMBED && options->input_file == NULL) {
        LOG(ERROR, "You must specify an input file with -in when embedding.")
        print_usage(argv[0]);
        return 0;
    }

    // If not password passed, set algorithm and mode to none
    if(strlen(options->password) == 0) {
        options->encryption_algo = ENC_NONE;
        options->encryption_mode = ENC_MODE_NONE;
        LOG(WARNING, "[arguments] No password passed. Setting encryption algorithm and mode to none.")
    }else{
        // Set default values for encryption algorithm and mode, and log level
        if (options->encryption_algo == ENC_NONE) {
            options->encryption_algo = DEFAULT_ENCRYPTION_ALGO;
            LOG(WARNING, "[arguments] No encryption algorithm specified. Using default: %s", encryption_algorithm_to_string(DEFAULT_ENCRYPTION_ALGO))
        }
        if(options->encryption_mode == ENC_MODE_NONE) {
            options->encryption_mode = DEFAULT_ENCRYPTION_MODE;
            LOG(WARNING, "[arguments] No encryption mode specified. Using default: %s", encryption_mode_to_string(DEFAULT_ENCRYPTION_MODE))
        }
    }

    LOG(DEBUG, "[arguments] All validations passed.")
    return 1;
}

void log_program_options(const ProgramOptions *options) {
    if (options == NULL) {
        LOG(ERROR, "Invalid ProgramOptions pointer.")
        return;
    }

    LOG(INFO, "[arguments] Arguments parsed successfully.")
    LOG(INFO, "[arguments] Program options:")

    LOG(INFO, "\t |-> Operation mode: %s", operation_mode_to_string(options->mode))

    if (options->input_file != NULL) {
        LOG(INFO, "\t |-> Input file: %s", options->input_file)
    } else {
        LOG(INFO, "\t |-> Input file: None")
    }

    if (options->input_bmp_file != NULL) {
        LOG(INFO, "\t |-> Input BMP file: %s", options->input_bmp_file)
    } else {
        LOG(INFO, "\t |-> Input BMP file: None")
    }

    if (options->output_file != NULL) {
        LOG(INFO, "\t |-> Output BMP file: %s", options->output_file)
    } else {
        LOG(INFO, "\t |-> Output BMP file: None")
    }

    LOG(INFO, "\t |-> Steganography algorithm: %s", steg_algorithm_to_string(options->steg_algorithm))
    LOG(DEBUG, "\t |-> Encryption algorithm: %s", encryption_algorithm_to_string(options->encryption_algo))
    LOG(DEBUG, "\t |-> Encryption mode: %s", encryption_mode_to_string(options->encryption_mode))

    if (strlen(options->password) > 0) {
        LOG(INFO, "\t |-> Password: %s", options->password)
    } else {
        LOG(INFO, "\t |-> Password: None")
    }
}

int parse_log_level_argument(int argc, char *argv[]){
    // Parse log level
    for (int i = 0; i < argc; i++) {
        // Check if log level is passed
        if (strcmp(argv[i], "-loglevel") == 0) {
            // Parse log level
            LogLevel logLevel = parse_log_level(argv[i + 1]);
            if(logLevel == NONE) {
                // Invalid log level
                print_usage(argv[0]);
                return 0;
            }
            // Set log level
            set_log_level(logLevel);
            break;
        } else{
            // Set default log level
            set_log_level(DEFAULT_LOG_LEVEL);
        }
    }
    return 1;
}

void print_usage(const char *program_name) {
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("\nEmbebido de la información:\n");
    printf("  -embed                  Ocultar información en un archivo BMP.\n");
    printf("  -in <file>              Archivo que se va a ocultar.\n");
    printf("  -p <bitmapfile>         Archivo BMP portador.\n");
    printf("  -out <bitmapfile>       Archivo BMP de salida.\n");
    printf("  -steg <LSB1|LSB4|LSBI>  Algoritmo de esteganografía.\n");
    printf("\nRecupero de la información:\n");
    printf("  -extract                Extraer información de un archivo BMP.\n");
    printf("  -p <bitmapfile>         Archivo BMP portador.\n");
    printf("  -out <file>             Archivo de salida obtenido.\n");
    printf("  -steg <LSB1|LSB4|LSBI>  Algoritmo de esteganografía.\n");
    printf("\nOpcionales:\n");
    printf("  -a <aes128 | aes192 | aes256 | 3des>      Algoritmo de encriptación. Default: %s\n", encryption_algorithm_to_string(DEFAULT_ENCRYPTION_ALGO));
    printf("  -m <ecb | cfb | ofb | cbc>                Modo de encriptación. Default: %s\n", encryption_mode_to_string(DEFAULT_ENCRYPTION_MODE));
    printf("  -pass <password>                          Contraseña para la encriptación.\n");
    printf("  -loglevel <DEBUG | INFO | ERROR | FATAL>  Nivel de log. Default: %s\n", log_level_to_string(DEFAULT_LOG_LEVEL));
    printf("\n");
}

OperationMode parse_operation_mode(const char *str) {
    if (strcmp(str, "embed") == 0) {
        return MODE_EMBED;
    } else if (strcmp(str, "extract") == 0) {
        return MODE_EXTRACT;
    } else {
        LOG(ERROR, "Invalid operation mode: %s.", str)
        return MODE_NONE;
    }
}

StegAlgorithm parse_steg_algorithm(const char *str) {
    if (strcmp(str, "LSB1") == 0) {
        return STEG_LSB1;
    } else if (strcmp(str, "LSB4") == 0) {
        return STEG_LSB4;
    } else if (strcmp(str, "LSBI") == 0) {
        return STEG_LSBI;
    } else {
        LOG(ERROR, "Invalid steganography algorithm: %s.", str)
        return STEG_NONE;
    }
}

EncryptionAlgorithm parse_encryption_algorithm(const char *str) {
    if (strcmp(str, "aes128") == 0) {
        return ENC_AES128;
    } else if (strcmp(str, "aes192") == 0) {
        return ENC_AES192;
    } else if (strcmp(str, "aes256") == 0) {
        return ENC_AES256;
    } else if (strcmp(str, "3des") == 0) {
        return ENC_3DES;
    } else {
        LOG(ERROR, "Invalid encryption algorithm: %s.", str)
        return ENC_NONE;
    }
}

EncryptionMode parse_encryption_mode(const char *str) {
    if (strcmp(str, "ecb") == 0) {
        return ENC_MODE_ECB;
    } else if (strcmp(str, "cfb") == 0) {
        return ENC_MODE_CFB;
    } else if (strcmp(str, "ofb") == 0) {
        return ENC_MODE_OFB;
    } else if (strcmp(str, "cbc") == 0) {
        return ENC_MODE_CBC;
    } else {
        LOG(ERROR, "Invalid encryption mode: %s.", str)
        return ENC_MODE_NONE;
    }
}

const char* operation_mode_to_string(OperationMode mode) {
    switch (mode) {
        case MODE_EMBED: return "embed";
        case MODE_EXTRACT: return "extract";
        default: return "UNKNOWN";
    }
}

const char* steg_algorithm_to_string(StegAlgorithm alg) {
    switch (alg) {
        case STEG_LSB1: return "LSB1";
        case STEG_LSB4: return "LSB4";
        case STEG_LSBI: return "LSBI";
        default: return "UNKNOWN";
    }
}

const char* encryption_algorithm_to_string(EncryptionAlgorithm alg) {
    switch (alg) {
        case ENC_AES128: return "aes128";
        case ENC_AES192: return "aes192";
        case ENC_AES256: return "aes256";
        case ENC_3DES: return "3des";
        default: return "UNKNOWN";
    }
}

const char* encryption_mode_to_string(EncryptionMode mode) {
    switch (mode) {
        case ENC_MODE_ECB: return "ecb";
        case ENC_MODE_CFB: return "cfb";
        case ENC_MODE_OFB: return "ofb";
        case ENC_MODE_CBC: return "cbc";
        default: return "UNKNOWN";
    }
}
