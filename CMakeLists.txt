cmake_minimum_required(VERSION 3.28)
project(StegoBMP C)

# Establecer el estándar de C
set(CMAKE_C_STANDARD 11)

# Incluir directorios donde se encuentran los headers
include_directories(src/include)

# Incluir directorios de la librería de CUnit
file(GLOB SRC_FILES "src/*.c")

# Crear el ejecutable principal StegoBMP
add_executable(StegoBMP
        ${SRC_FILES}
)

# ------ Configuración de Tests ------
# Habilitar la funcionalidad de pruebas de CMake
enable_testing()

# Crear un ejecutable para las pruebas, asegurando que incluya file_handler.c
add_executable(test_file_handler
        tests/test_file_handler.c
        src/logger.c
        src/file_handler.c
)

# Definir TESTING solo para los tests
target_compile_definitions(test_file_handler PRIVATE TESTING)

# Agregar las pruebas al sistema de tests
add_test(NAME FileHandlerTest COMMAND test_file_handler)



