//
// Created by Felipe Cupitó on 30/09/2024.
//
#include "logger.h"

/**
 * @brief Nivel de log actual. Por defecto, se establece en DEBUG para máxima información.
 */
LogLevel log_level = DEBUG;

void set_log_level(LogLevel new_level) {
    if (new_level >= DEBUG && new_level <= FATAL) {
        log_level = new_level;
    }
}

const char *get_log_level_description(LogLevel level) {
    static const char *level_descriptions[] = {"DEBUG", "INFO", "ERROR", "FATAL"};
    if (level < DEBUG || level > FATAL) {
        return "UNKNOWN";
    }
    return level_descriptions[level];
}
