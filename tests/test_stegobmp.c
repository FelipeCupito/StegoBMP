

#include <stdio.h>
#include <stdlib.h>
#include "../src/include/stego_bmp.h"
#include "../src/include/file_handler.h"

#define IMG_BASE_PATH "../resources/test_images/"


void test_1(){

    FilePackage*  file = create_file_package("../resources/test_images/data.txt");
    BMPImage* bmp = read_bmp_file("../resources/test_images/sample1.bmp");

    BMPImage* bmp2 = embed(bmp, file, STEG_LSB1);


    save_bmp_file("../resources/test_images/sample1_teste.bmp", bmp2);

}

int main() {
    test_1();
}