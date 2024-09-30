# StegoBMP

## Descripción del Proyecto

**StegoBMP** es un proyecto que permite trabajar con archivos de imagen BMP y realizar operaciones de esteganografía. El proyecto está desarrollado en C y utiliza **CMake** para la configuración y compilación del código, y **CTest** para realizar pruebas unitarias.

## Requisitos Previos

Antes de compilar y correr el proyecto, asegúrate de tener los siguientes requisitos instalados:

- **CMake** (Versión 3.28 o superior)
- Un compilador de C (por ejemplo, GCC o Clang)
- **Make** (si estás en un sistema basado en Unix como Linux o macOS)

### Instalación de CMake

#### En Linux (Debian/Ubuntu):

```bash
sudo apt update
sudo apt install cmake
```

#### En macOS con Homebrew:

```bash
brew install cmake
```

#### En Windows:

Puedes descargar CMake desde el [sitio oficial de CMake](https://cmake.org/download/) y seguir las instrucciones de instalación.


## Instrucciones para Compilar el Proyecto

### Paso 1: Crear el Directorio de Construcción

Primero, necesitas crear un directorio `build/` donde se generarán los archivos de construcción.

```bash
mkdir build
cd build
```

### Paso 2: Configurar el Proyecto con CMake

Dentro del directorio `build`, ejecuta el siguiente comando para que CMake configure el proyecto:

```bash
cmake ..
```

Esto leerá el archivo `CMakeLists.txt` y configurará todo lo necesario para compilar el proyecto y los tests.

### Paso 3: Compilar el Proyecto y los Tests

Una vez que el proyecto esté configurado, compílalo ejecutando:

```bash
make
```

Esto compilará tanto el ejecutable principal **StegoBMP** como el ejecutable de pruebas **test_file_handler**.

## Instrucciones para Ejecutar el Programa

Después de compilar, puedes ejecutar el programa principal **StegoBMP** desde el directorio `build`:

```bash
./StegoBMP
```

Este comando ejecutará el código principal del programa.

## Instrucciones para Ejecutar las Pruebas

El proyecto incluye pruebas unitarias que puedes ejecutar utilizando **CTest**. Aquí te muestro dos formas de hacerlo.

### Usar CTest

Para correr las pruebas, simplemente ejecuta el siguiente comando desde el directorio `build`:

```bash
ctest --verbose
```

Esto ejecutará todas las pruebas registradas y te mostrará los resultados con detalles.

## Limpiar los Archivos Generados

Si necesitas limpiar los archivos de compilación generados por **CMake**, puedes ejecutar el siguiente comando:

```bash
make clean
```

Esto eliminará los archivos binarios generados y te permitirá comenzar desde cero.

## Contribuciones

falta...

