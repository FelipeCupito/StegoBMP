
#include <stdio.h>
#include <stdlib.h>
#include "./include/logger.h"
#include "./include/file_handler.h"
#include "./include/arguments.h"

int main(int argc, char *argv[]) {
    // Parse command-line arguments
    ProgramOptions arguments;
    if (!parse_arguments(argc, argv, &arguments)) {
        LOG(ERROR, "Error parsing arguments.");
        return 1;
    }

    // Log the parsed arguments
    log_program_options(&arguments);

    // Check the operation mode
    if (arguments.mode == MODE_EMBED) {
        LOG(INFO, "Embedding mode selected.");

        // Load the input file
        FilePackage *package = create_file_package(arguments.input_file);
        LOG(INFO, "File loaded successfully.")

        //TODO: implementar la inserción





    } else if (arguments.mode == MODE_EXTRACT) {

        //TODO: implementar la extracción


        FilePackage *package;
        if(!create_file_from_package(arguments.output_file, package)){
            LOG(ERROR, "Error creating the output file.");
            return 1;
        }
        LOG(INFO, "Output file created successfully.")
    }


    LOG(INFO, "Operation completed successfully.");
    return 0;
}


