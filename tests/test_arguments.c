#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/include/arguments.h"


void print_test_result(const char *test_name) {
        printf("Test %s passed.\n", test_name);
}

void test_parse_embed_arguments() {
    char *argv[] = {
            "stegobmp", "-embed", "-in", "input.txt", "-p", "carrier.bmp",
            "-out", "output.bmp", "-steg", "LSB1"
    };
    int argc = sizeof(argv) / sizeof(char*);

    ProgramOptions options;
    optind = 1;  // Reiniciar optind antes de cada test
    int result = parse_arguments(argc, argv, &options);

    // Validate the parsing
    assert(result == 1);
    assert(options.mode == MODE_EMBED);
    assert(strcmp(options.input_file, "input.txt") == 0);
    assert(strcmp(options.input_bmp_file, "carrier.bmp") == 0);
    assert(strcmp(options.output_file, "output.bmp") == 0);
    assert(options.steg_algorithm == STEG_LSB1);
    assert(options.encryption_algo == ENC_NONE);
    assert(options.encryption_mode == ENC_MODE_NONE);
    assert(strcmp(options.password, "") == 0);


    print_test_result("test_parse_embed_arguments");
}

void test_embed_in_is_missing() {
    char *argv[] = {
            "stegobmp", "-embed", "-p", "carrier.bmp",
            "-out", "output.bmp", "-steg", "LSB1"
    };
    int argc = sizeof(argv) / sizeof(char*);

    ProgramOptions options;
    optind = 1;  // Reiniciar optind antes de cada test
    int result = parse_arguments(argc, argv, &options);
    assert(result == 0);
    print_test_result("test_embed_in_is_missing");
}

void test_embed_p_is_missing() {
    char *argv[] = {
            "stegobmp", "-embed", "-in", "input.txt",
            "-out", "output.bmp", "-steg", "LSB1"
    };
    int argc = sizeof(argv) / sizeof(char*);

    ProgramOptions options;
    optind = 1;  // Reiniciar optind antes de cada test
    int result = parse_arguments(argc, argv, &options);
    assert(result == 0);
    print_test_result("test_embed_p_is_missing");
}

void test_embed_out_is_missing() {
    char *argv[] = {
            "stegobmp", "-embed", "-in", "input.txt", "-p", "carrier.bmp",
            "-steg", "LSB1"
    };
    int argc = sizeof(argv) / sizeof(char*);

    ProgramOptions options;
    optind = 1;  // Reiniciar optind antes de cada test
    int result = parse_arguments(argc, argv, &options);
    assert(result == 0);
    print_test_result("test_embed_out_is_missing");
}

void test_embed_steg_is_missing() {
    char *argv[] = {
            "stegobmp", "-embed", "-in", "input.txt", "-p", "carrier.bmp",
            "-out", "output.bmp"
    };
    int argc = sizeof(argv) / sizeof(char*);

    ProgramOptions options;
    optind = 1;  // Reiniciar optind antes de cada test
    int result = parse_arguments(argc, argv, &options);
    assert(result == 0);
    print_test_result("test_embed_steg_is_missing");
}

void test_parse_extract_arguments_example_1() {
    /* Ejemplo 1 del enunciado */
    char *argv[] = {
            "stegobmp", "-extract", "-p", "imagen1.bmp",
            "-out", "imagenmas1.bmp", "-steg", "LSBI", "-a", "3des",
            "-m", "cbc", "-pass", "oculto"
    };
    int argc = sizeof(argv) / sizeof(char*);

    ProgramOptions options;
    optind = 1;  // Reiniciar optind antes de cada test
    int result = parse_arguments(argc, argv, &options);

    // Validate the parsing
    assert(result == 1);
    assert(options.mode == MODE_EXTRACT);
    assert(options.input_file == NULL);
    assert(strcmp(options.input_bmp_file, "imagen1.bmp") == 0);
    assert(strcmp(options.output_file, "imagenmas1.bmp") == 0);
    assert(options.steg_algorithm == STEG_LSBI);
    assert(options.encryption_algo == ENC_3DES);
    assert(options.encryption_mode == ENC_MODE_CBC);
    assert(strcmp(options.password, "oculto") == 0);

    print_test_result("test_parse_extract_arguments_example_1");
}

void test_parse_extract_arguments_example_2() {
    /* Ejemplo 2 del enunciado */
    char *argv[] = {
            "stegobmp", "-extract", "-p", "imagen1.bmp",
            "-out", "imagenmas1.bmp", "-steg", "LSBI"
    };
    int argc = sizeof(argv) / sizeof(char*);

    ProgramOptions options;
    optind = 1;  // Reiniciar optind antes de cada test
    int result = parse_arguments(argc, argv, &options);

    // Validate the parsing
    assert(result == 1);
    assert(options.mode == MODE_EXTRACT);
    assert(options.input_file == NULL);
    assert(strcmp(options.input_bmp_file, "imagen1.bmp") == 0);
    assert(strcmp(options.output_file, "imagenmas1.bmp") == 0);
    assert(options.steg_algorithm == STEG_LSBI);
    assert(options.encryption_algo == ENC_NONE);
    assert(options.encryption_mode == ENC_MODE_NONE);
    assert(strcmp(options.password, "") == 0);
    print_test_result("test_parse_extract_arguments_example_2");
}

void test_parse_extract_arguments_example_3() {
    /* $stegobmp –extract –p “imagenmas1 .bmp” -out “mensaje1” –steg LSBI –a 3des –m cbc -pass
“oculto” */
    char *argv[] = {
            "stegobmp", "-extract", "-p", "imagenmas1.bmp",
            "-out", "mensaje1", "-steg", "LSBI", "-a", "3des",
            "-m", "cbc", "-pass", "oculto"
    };
    int argc = sizeof(argv) / sizeof(char*);

    ProgramOptions options;
    optind = 1;  // Reiniciar optind antes de cada test
    int result = parse_arguments(argc, argv, &options);

    // Validate the parsing
    assert(result == 1);
    assert(options.mode == MODE_EXTRACT);
    assert(options.input_file == NULL);
    assert(strcmp(options.input_bmp_file, "imagenmas1.bmp") == 0);
    assert(strcmp(options.output_file, "mensaje1") == 0);
    assert(options.steg_algorithm == STEG_LSBI);
    assert(options.encryption_algo == ENC_3DES);
    assert(options.encryption_mode == ENC_MODE_CBC);
    assert(strcmp(options.password, "oculto") == 0);
    print_test_result("test_parse_extract_arguments_example_3");
}

void test_optional_arg_extract_not_pass(){
    char *argv[] = {
            "stegobmp", "-extract", "-p", "imagenmas1.bmp",
            "-out", "mensaje1", "-steg", "LSBI", "-a", "3des",
            "-m", "cbc"
    };
    int argc = sizeof(argv) / sizeof(char*);

    ProgramOptions options;
    optind = 1;  // Reiniciar optind antes de cada test
    int result = parse_arguments(argc, argv, &options);

    // Validate the parsing
    assert(result == 1);
    assert(options.mode == MODE_EXTRACT);
    assert(options.input_file == NULL);
    assert(strcmp(options.input_bmp_file, "imagenmas1.bmp") == 0);
    assert(strcmp(options.output_file, "mensaje1") == 0);
    assert(options.steg_algorithm == STEG_LSBI);
    assert(options.encryption_algo == ENC_NONE);
    assert(options.encryption_mode == ENC_MODE_NONE);
    assert(strcmp(options.password, "") == 0);

    print_test_result("test_optional_arguments_not_pass");
}

void test_optional_embed_not_pass(){
    char *argv[] = {
            "stegobmp", "-embed", "-in", "input.txt", "-p", "carrier.bmp",
            "-out", "output.bmp", "-steg", "LSB1"
    };
    int argc = sizeof(argv) / sizeof(char*);

    ProgramOptions options;
    optind = 1;  // Reiniciar optind antes de cada test
    int result = parse_arguments(argc, argv, &options);

    // Validate the parsing
    assert(result == 1);
    assert(options.mode == MODE_EMBED);
    assert(strcmp(options.input_file, "input.txt") == 0);
    assert(strcmp(options.input_bmp_file, "carrier.bmp") == 0);
    assert(strcmp(options.output_file, "output.bmp") == 0);
    assert(options.steg_algorithm == STEG_LSB1);
    assert(options.encryption_algo == ENC_NONE);
    assert(options.encryption_mode == ENC_MODE_NONE);
    assert(strcmp(options.password, "") == 0);

    print_test_result("test_optional_arguments_not_pass");
}

void test_optional_mode() {
    char *argv[] = {
            "stegobmp", "-embed", "-in", "input.txt", "-p", "carrier.bmp",
            "-out", "output.bmp", "-steg", "LSB1", "-pass", "oculto"
    };
    int argc = sizeof(argv) / sizeof(char*);

    ProgramOptions options;
    optind = 1;  // Reiniciar optind antes de cada test
    int result = parse_arguments(argc, argv, &options);

    // Validate the parsing
    assert(result == 1);
    assert(options.mode == MODE_EMBED);
    assert(strcmp(options.input_file, "input.txt") == 0);
    assert(strcmp(options.input_bmp_file, "carrier.bmp") == 0);
    assert(strcmp(options.output_file, "output.bmp") == 0);
    assert(options.steg_algorithm == STEG_LSB1);
    assert(options.encryption_algo == ENC_AES128);
    assert(options.encryption_mode == ENC_MODE_CBC);
    assert(strcmp(options.password, "oculto") == 0);
    print_test_result("test_optional_mode");
}

void test_optinal_algorithm() {
    char *argv[] = {
            "stegobmp", "-embed", "-in", "input.txt", "-p", "carrier.bmp",
            "-out", "output.bmp", "-steg", "LSB1", "-pass", "oculto", "-m", "cbc"
    };
    int argc = sizeof(argv) / sizeof(char*);

    ProgramOptions options;
    optind = 1;  // Reiniciar optind antes de cada test
    int result = parse_arguments(argc, argv, &options);

    // Validate the parsing
    assert(result == 1);
    assert(options.mode == MODE_EMBED);
    assert(strcmp(options.input_file, "input.txt") == 0);
    assert(strcmp(options.input_bmp_file, "carrier.bmp") == 0);
    assert(strcmp(options.output_file, "output.bmp") == 0);
    assert(options.steg_algorithm == STEG_LSB1);
    assert(options.encryption_algo == ENC_AES128);
    assert(options.encryption_mode == ENC_MODE_CBC);
    assert(strcmp(options.password, "oculto") == 0);
    print_test_result("test_optinal_algorithm");
}

void test_optional_only_pass(){
    char *argv[] = {
            "stegobmp", "-embed", "-in", "input.txt", "-p", "carrier.bmp",
            "-out", "output.bmp", "-steg", "LSB1", "-pass", "oculto"
    };
    int argc = sizeof(argv) / sizeof(char*);

    ProgramOptions options;
    optind = 1;  // Reiniciar optind antes de cada test
    int result = parse_arguments(argc, argv, &options);

    // Validate the parsing
    assert(result == 1);
    assert(options.mode == MODE_EMBED);
    assert(strcmp(options.input_file, "input.txt") == 0);
    assert(strcmp(options.input_bmp_file, "carrier.bmp") == 0);
    assert(strcmp(options.output_file, "output.bmp") == 0);
    assert(options.steg_algorithm == STEG_LSB1);
    assert(options.encryption_algo == ENC_AES128);
    assert(options.encryption_mode == ENC_MODE_CBC);
    assert(strcmp(options.password, "oculto") == 0);
    print_test_result("test_optional_only_pass");
}

/**
 * @brief Test case for converting strings to enums.
 */
void test_parse_enums() {
    // Test operation mode
    assert(parse_operation_mode("embed") == MODE_EMBED);
    assert(parse_operation_mode("extract") == MODE_EXTRACT);
    assert(parse_operation_mode("invalid") == MODE_NONE);

    // Test steganography algorithms
    assert(parse_steg_algorithm("LSB1") == STEG_LSB1);
    assert(parse_steg_algorithm("LSB4") == STEG_LSB4);
    assert(parse_steg_algorithm("LSBI") == STEG_LSBI);
    assert(parse_steg_algorithm("invalid") == STEG_NONE);

    // Test encryption algorithms
    assert(parse_encryption_algorithm("aes128") == ENC_AES128);
    assert(parse_encryption_algorithm("aes192") == ENC_AES192);
    assert(parse_encryption_algorithm("aes256") == ENC_AES256);
    assert(parse_encryption_algorithm("3des") == ENC_3DES);
    assert(parse_encryption_algorithm("invalid") == ENC_NONE);

    // Test encryption modes
    assert(parse_encryption_mode("ecb") == ENC_MODE_ECB);
    assert(parse_encryption_mode("cfb") == ENC_MODE_CFB);
    assert(parse_encryption_mode("ofb") == ENC_MODE_OFB);
    assert(parse_encryption_mode("cbc") == ENC_MODE_CBC);
    assert(parse_encryption_mode("invalid") == ENC_MODE_NONE);

    print_test_result("test_parse_enums");
}

int main() {
    set_log_level(DEBUG);

    //Run all test cases
    test_parse_embed_arguments();
    test_embed_in_is_missing();
    test_embed_p_is_missing();
    test_embed_out_is_missing();
    test_embed_steg_is_missing();
    test_parse_extract_arguments_example_1();
    test_parse_extract_arguments_example_2();
    test_parse_extract_arguments_example_3();
    test_optional_arg_extract_not_pass();
    test_optional_embed_not_pass();
    test_optional_mode();
    test_optinal_algorithm();
    test_optional_only_pass();
    test_parse_enums();

    printf("All tests completed.\n");

    return 0;
}
