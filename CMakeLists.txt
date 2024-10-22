cmake_minimum_required(VERSION 3.28)
project(StegoBMP C)

# Establecer el estándar de C
set(CMAKE_C_STANDARD 11)

# Incluir directorios donde se encuentran los headers
include_directories(src/include)

# Buscar la librería OpenSSL
find_package(OpenSSL REQUIRED)

# Incluir directorios de la librería de CUnit
file(GLOB SRC_FILES "src/*.c")

# Crear el ejecutable principal StegoBMP
add_executable(stegobmp
        ${SRC_FILES}
)

# Enlazar el ejecutable con OpenSSL
target_link_libraries(stegobmp OpenSSL::SSL OpenSSL::Crypto)


# ------ Configuración de Tests ------ #
enable_testing()

# file handler tests
add_executable(test_file_handler
        tests/test_file_handler.c
        src/logger.c
        src/file_handler.c
)
target_compile_definitions(test_file_handler PRIVATE TESTING)
add_test(NAME FileHandlerTest COMMAND test_file_handler)

# argumnets tests
add_executable(test_arguments
        tests/test_arguments.c
        src/logger.c
        src/arguments.c
)
target_compile_definitions(test_arguments PRIVATE TESTING)
add_test(NAME ArgumentsTest COMMAND test_arguments)

# stegobmp tests
add_executable(test_stegobmp
        tests/test_stegobmp.c
        src/logger.c
        src/stego_bmp.c
        src/file_handler.c
)
target_compile_definitions(test_stegobmp PRIVATE TESTING)
add_test(NAME StegoBMPTest COMMAND test_stegobmp)



