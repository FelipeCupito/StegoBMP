#include <stdio.h>
#include <stdlib.h>
#include "../src/include/stego_bmp.h"
#include "../src/include/file_handler.h"

#define IMG_BASE_PATH "../resources/test_images/"


void test_embed_algorithms(const char* bmp_path, const char* bmp_output_path, const char* msg_path, StegAlgorithm algorithm) {

    FilePackage*  file = create_file_package(msg_path);
    BMPImage* bmp = read_bmp_file(bmp_path);

    BMPImage* bmp_embed = embed(bmp, file, algorithm);


    save_bmp_file(bmp_output_path, bmp_embed);

}

int main() {
    test_embed_algorithms("../resources/test_images/sample1.bmp", "../resources/test_images/sample1_test.bmp",
                          "../resources/test_images/data.txt", STEG_LSB1);
    test_embed_algorithms("../resources/test_images/sample2.bmp", "../resources/test_images/sample2_test.bmp",
                          "../resources/test_images/data.txt", STEG_LSB4);
    test_embed_algorithms("../resources/test_images/sample3.bmp", "../resources/test_images/sample3_test.bmp",
                          "../resources/test_images/data.txt", STEG_LSBI);
    printf("test_stegobmp passed.\n");
    return 0;
}