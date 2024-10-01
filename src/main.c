
#include <stdio.h>
#include <stdlib.h>
#include "./include/logger.h"
#include "./include/file_handler.h"
#include "./include/arguments.h"


int main(int argc, char *argv[]) {
    LOG(INFO, "StegoBMP started.")
    // Procesa los argumentos
    ProgramOptions options;
    if (!parse_arguments(argc, argv, &options)) {
        LOG(ERROR, "Error parsing arguments.");
        return 1;
    }
    LOG(INFO, "Arguments parsed successfully.")

    // Establecer el nivel de log de acuerdo a los argumentos
    set_log_level(options.log_level);
    LOG(INFO, "Log level set to %s.", log_level_to_string(log_level));

    if (options.mode == MODE_EMBED) {
        LOG(INFO, "Embedding mode selected.");
        FilePackage *package = create_file_package(options.input_file);
        LOG(INFO, "File loaded successfully.")

        //TODO: borra esta línea
        print_file_package(package);

        //TODO: implementar la inserción

    } else if (options.mode == MODE_EXTRACT) {
        //TODO: implementar la extracción
    }

    LOG(INFO, "Operation completed successfully.");
    return 0;
}


