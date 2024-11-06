# StegoBMP

## Descripción

**StegoBMP** es una herramienta de esteganografía en C que permite ocultar y extraer archivos en imágenes BMP mediante varios algoritmos de inserción en el bit menos significativo (LSB). Soporta cifrado opcional utilizando OpenSSL con métodos como AES y Triple DES. Este programa permite realizar las operaciones de **ocultamiento** y **extracción** en archivos BMP de 24 bits por píxel sin compresión.

## Objetivos

- **Ocultar archivos** en imágenes BMP usando algoritmos de esteganografía (LSB1, LSB4, LSBI).
- **Extraer archivos** ocultos de una imagen BMP.
- **Encriptación** opcional de los datos ocultos utilizando OpenSSL.

## Estructura del Proyecto

- **`arguments.h`**: Define las opciones de línea de comandos, como el archivo de entrada, salida, algoritmo de esteganografía y cifrado.
- **`bmp_image.h`**: Maneja la lectura y escritura de archivos BMP, y valida que sean del formato adecuado.
- **`crypto.h`**: Proporciona funciones de cifrado y descifrado utilizando OpenSSL.
- **`file_package.h`**: Permite gestionar la carga, empaquetado y extracción de archivos en memoria.
- **`stego_embed.h`**: Funciones principales de esteganografía para insertar y extraer datos de las imágenes BMP.
- **`types.h`**: Enumera tipos y estructuras clave, como modos de operación y algoritmos de cifrado.
- **`utils.h`**: Funciones auxiliares para ajustar la endianess, manejo de archivos y obtención de extensiones.


## Requisitos Previos

Antes de compilar y correr el proyecto, asegúrate de tener los siguientes requisitos instalados:

- **CMake** (Versión 3.28 o superior)
- Un compilador de C (por ejemplo, GCC o Clang)
- **Make** (si estás en un sistema basado en Unix como Linux o macOS)
- **OpenSSL** (para las operaciones criptográficas)

### Instalación de OpenSSL

El proyecto **StegoBMP** requiere **OpenSSL** para realizar operaciones de criptografía. Asegúrate de tenerlo instalado en tu sistema.

#### En Linux (Debian/Ubuntu):

```bash
sudo apt-get update
sudo apt-get install libssl-dev
```

#### En macOS con Homebrew:

```bash
brew install openssl
```

#### En Windows:

Puedes descargar e instalar OpenSSL desde el [sitio oficial de OpenSSL](https://slproweb.com/products/Win32OpenSSL.html). Asegúrate de que las rutas de OpenSSL estén correctamente configuradas en tu entorno.

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

## Compilación e Instalación

Primero, asegúrese de contar con OpenSSL y CMake instalados. Para compilar el programa, ejecute:

```bash
mkdir build
cd build
cmake ..
make
```

Esto generará el ejecutable `stegobmp`.

## Uso

El programa **StegoBMP** permite dos modos de operación: ocultamiento y extracción.

### 1. Ocultamiento

Para ocultar un archivo en una imagen BMP, utilice el comando `-embed`:

```bash
./stegobmp -embed -in <archivo_a_ocultar> -p <imagen_bmp> -out <imagen_bmp_salida> -steg <LSB1 | LSB4 | LSBI> [-a <aes128 | aes192 | aes256 | 3des>] [-m <ecb | cfb | ofb | cbc>] [-pass <contraseña>]
```

#### Ejemplo

```bash
./stegobmp -embed -in mensaje.txt -p imagen.bmp -out imagen_oculta.bmp -steg LSBI -a aes128 -m cbc -pass "secreto"
```

### 2. Extracción

Para extraer un archivo oculto de una imagen BMP, use:

```bash
./stegobmp -extract -p <imagen_bmp_con_datos_ocultos> -out <archivo_extraido> -steg <LSB1 | LSB4 | LSBI> [-a <aes128 | aes192 | aes256 | 3des>] [-m <ecb | cfb | ofb | cbc>] [-pass <contraseña>]
```

#### Ejemplo

```bash
./stegobmp -extract -p imagen_oculta.bmp -out mensaje_extraido.txt -steg LSBI -a aes128 -m cbc -pass "secreto"
```

## Notas Adicionales

- **Formato Endian**: Todos los tamaños (`size`) embebidos en los datos se almacenan en formato big-endian. Si se desea utilizar little-endian, es necesario cambiar la constante `IS_DATA_BIG_ENDIAN` a `false` en `utils.h`.
- Solo se permite encriptar si se proporciona una contraseña.
- Incluye un nivel de log configurable para facilitar la depuración (`DEBUG`, `INFO`, `ERROR`, `FATAL`).


## Contribuciones

 - Cupitó, Felipe - 60058
 - Podgorny, Andres - 60570 
 - De Caro, Guido - 61590
