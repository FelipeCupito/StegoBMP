#ifndef STEGOBMP_LOGGER_H
#define STEGOBMP_LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Niveles de log para la salida de mensajes de depuración e información.
 */
typedef enum {
    DEBUG = 0,  /**< Nivel para mensajes de depuración detallados. */
    INFO,       /**< Nivel para mensajes informativos. */
    WARNING,    /**< Nivel para mensajes de advertencia. */
    ERROR,      /**< Nivel para mensajes de errores que no terminan el programa. */
    FATAL,      /**< Nivel para errores críticos que terminan el programa. */
    NONE        /**< Nivel para desactivar los mensajes de log. */
} LogLevel;

/**
 * @brief Nivel de log actual. Solo los mensajes con nivel mayor o igual a este serán registrados.
 */
extern LogLevel log_level;

/**
 * @brief Cambia el nivel mínimo de severidad para registrar los mensajes de log.
 *
 * @param new_level Nivel mínimo de log a registrar. Los mensajes con nivel menor serán ignorados.
 */
void set_log_level(LogLevel new_level);

/**
 * @brief Retorna una descripción en cadena del nivel de log.
 *
 * @param level Nivel de log.
 * @return Cadena con la descripción textual del nivel de log.
 */
const char* log_level_to_string(LogLevel level);

/**
 * @brief Parcea una cadena a un nivel de log.
 *
 * @param level Cadena con el nivel de log.
 * @return LogLevel de log correspondiente a la cadena, o `NONE` si no se reconoce.
 */
LogLevel parse_log_level(const char *level);


/**
 * @brief Macro para registrar un mensaje de log.
 *
 * Los mensajes `INFO` y `DEBUG` se envían a `stdout`, mientras que los mensajes `ERROR` y `FATAL` se envían a `stderr`.
 * Los metadatos de archivo y línea solo se incluyen para los niveles `ERROR` y `FATAL`.
 *
 * @param level Nivel de severidad del mensaje.
 * @param fmt Formato del mensaje (similar a printf).
 * @param ... Argumentos variables para el formato del mensaje.
 */
#define LOG(level, fmt, ...)   {\
    if(level >= log_level) {\
        FILE* output = (level == ERROR || level == FATAL) ? stderr : stdout; \
        if (level == ERROR || level == FATAL) {\
            fprintf(output, "%s: %s:%d: ", log_level_to_string(level), __FILE__, __LINE__); \
        } else {\
            fprintf(output, "%s: ", log_level_to_string(level)); \
        }\
        fprintf(output, fmt, ##__VA_ARGS__); \
        fprintf(output, "\n");\
        if (level == FATAL) exit(1);\
    }\
}

#endif //STEGOBMP_LOGGER_H
