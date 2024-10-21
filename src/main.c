
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
        FilePackage *package = new_file_package(arguments.input_file);
        if (package == NULL) {
            LOG(ERROR, "Error loading the input file.");
            return 1;
        }
        LOG(INFO, "File loaded successfully.")

        // Load the BMP file
        BMPImage *bmp = new_bmp_file(arguments.input_bmp_file);
        if (bmp == NULL) {
            LOG(ERROR, "Error loading the BMP file.");
            return 1;
        }
        LOG(INFO, "BMP file loaded successfully.")

        //ENCode the file
        //TODO: implementar la codificación

        // Embed the file into the BMP image
        //TODO: implementar la inserci





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


