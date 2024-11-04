
#include <stdio.h>
#include <stdlib.h>
#include "./include/logger.h"
#include "./include/bmp_image.h"
#include "./include/arguments.h"
#include "./include/file_package.h"
#include "./include/stego_bmp.h"
#include "./include/crypto.h"

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
            emd_data = crypto_encrypt(emd_data, size, arguments.encryption_algo, arguments.encryption_mode, arguments.password, &size);
        }

        // Embed the data into the BMP image
        embed(bmp, emd_data, size, arguments.steg_algorithm);

        // Save the BMP file
        int save_result = save_bmp_file(arguments.output_file, bmp);
        if (save_result != 0) {
            LOG(ERROR, "Error saving the BMP file.")
            return 1;
        }


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
            LOG(INFO, "Decrypting the extracted data.")
            size_t extracted_size = 0;
            uint8_t *encrypted_data = extract_encrypted_data(bmp, arguments.steg_algorithm, &extracted_size);
            if (encrypted_data == NULL) {
                LOG(ERROR, "Error extracting encrypted data.")
                return 1;
            }

            // Decrypt the extracted data
            uint8_t *decrypted_data = crypto_decrypt(encrypted_data, extracted_size, arguments.encryption_algo, arguments.encryption_mode, (const uint8_t *)arguments.password, &extracted_size);
            if (decrypted_data == NULL) {
                LOG(ERROR, "Error decrypting the extracted data.")
                return 1;
            }

            // Create a FilePackage from the decrypted data
            package = new_file_package_from_data(decrypted_data);
            if (package == NULL) {
                LOG(ERROR, "Error creating FilePackage from de crypted data.")
                return 1;
            }
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


