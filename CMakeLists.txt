cmake_minimum_required(VERSION 3.28)
project(StegoBMP C)

# Establecer el estándar de C
set(CMAKE_C_STANDARD 11)

# Buscar la librería OpenSSL
find_package(OpenSSL REQUIRED)

# Recopilar los archivos fuente de la biblioteca excluyendo main.c
file(GLOB LIB_SOURCES "src/*.c")
list(FILTER LIB_SOURCES EXCLUDE REGEX "src/main\.c$")

# Crear una biblioteca estática llamada stegolib
add_library(stegolib STATIC ${LIB_SOURCES})

# Incluir directorios donde se encuentran los headers
target_include_directories(stegolib PUBLIC src/include)

# Enlazar la biblioteca con OpenSSL
target_link_libraries(stegolib PUBLIC OpenSSL::SSL OpenSSL::Crypto)

# Crear el ejecutable principal StegoBMP
add_executable(stegobmp src/main.c)

# Enlazar el ejecutable principal con stegolib
target_link_libraries(stegobmp PRIVATE stegolib)

# ------ Configuración de Tests ------ #
enable_testing()

# Definir los archivos de prueba
set(TEST_SOURCES
        tests/test_bmp_image.c
        tests/test_arguments.c
        tests/test_stegobmp.c
        tests/test_file_package.c
)

# Crear ejecutables de prueba y enlazarlos con stegolib
foreach(test_src ${TEST_SOURCES})
    # Obtener el nombre del ejecutable sin la extensión
    get_filename_component(test_name ${test_src} NAME_WE)

    # Crear el ejecutable de prueba
    add_executable(${test_name} ${test_src})

    # Definir la macro TESTING para los tests
    target_compile_definitions(${test_name} PRIVATE TESTING)

    # Enlazar el ejecutable de prueba con stegolib y OpenSSL
    target_link_libraries(${test_name} PRIVATE stegolib)

    # Agregar el test a CTest
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
