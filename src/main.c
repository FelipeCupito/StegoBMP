
#include <stdio.h>
#include <stdlib.h>
#include "./include/logger.h"
#include "./include/bmp_image.h"
#include "./include/arguments.h"
#include "./include/file_package.h"
#include "./include/stego_bmp.h"

int main(int argc, char *argv[]) {
    // Parse command-line arguments
    ProgramOptions arguments;
    if (!parse_arguments(argc, argv, &arguments)) {
        LOG(ERROR, "Error parsing arguments.")
        return 1;
    }
    // Log the parsed arguments
    log_program_options(&arguments);

    // Check the operation mode
    if (arguments.mode == MODE_EMBED) {
        LOG(INFO, "Embedding mode selected.")

        // Load the input file
        size_t  size = 0;
        uint8_t *emd_data = embed_data_from_file(arguments.input_file, &size);

        // Load the BMP file
        BMPImage *bmp = new_bmp_file(arguments.input_bmp_file);
        if (bmp == NULL) {
            LOG(ERROR, "Error loading the BMP file.")
            return 1;
        }

        // Encrypt the data if necessary
        if(arguments.encryption_mode != ENC_NONE){
            LOG(INFO, "Encrypting the data.")
        }

        // Embed the data into the BMP image
        embed(bmp, emd_data, size, arguments.steg_algorithm);

        // Save the BMP file
        int save_result = save_bmp_file(arguments.output_file, bmp);
        if (save_result != 0) {
            LOG(ERROR, "Error saving the BMP file.")
            return 1;
        }
        LOG(INFO, "BMP file saved successfully.")


    } else if (arguments.mode == MODE_EXTRACT) {
        LOG(INFO, "Extraction mode selected.")

        // Load the BMP file
        BMPImage *bmp = new_bmp_file(arguments.input_bmp_file);
        if (bmp == NULL) {
            LOG(ERROR, "Error loading the BMP file.")
            return 1;
        }

        FilePackage *package = NULL;
        // Extract the data from the BMP image
        if(arguments.encryption_mode == ENC_NONE){
            package = extract_data(bmp, arguments.steg_algorithm);
            if (package == NULL) {
                LOG(ERROR, "Error extracting data.")
                return 1;
            }
        } else{
            LOG(ERROR, "Decrypting the data.sssssssss")
            //extract_encrypted_data(bmp, arguments.steg_algorithm, &package->size);

        }

        // Save the extracted data to a file
        if(!create_file_from_package(arguments.output_file, package)){
            LOG(ERROR, "Error creating the output file.")
            return 1;
        }

        LOG(INFO, "Output file created successfully.")
    }else{
        LOG(ERROR, "Invalid operation mode.")
        return 1;
    }

    return 0;
}


