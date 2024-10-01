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

const char *log_level_to_string(LogLevel level) {
    static const char *level_descriptions[] = {"DEBUG", "INFO", "ERROR", "FATAL"};
    if (level < DEBUG || level > FATAL) {
        return "UNKNOWN";
    }
    return level_descriptions[level];
}

LogLevel parse_log_level(const char *level){
    if (strcmp(level, "DEBUG") == 0) {
        return DEBUG;
    } else if (strcmp(level, "INFO") == 0) {
        return INFO;
    } else if (strcmp(level, "ERROR") == 0) {
        return ERROR;
    } else if (strcmp(level, "FATAL") == 0) {
        return FATAL;
    } else {
        return NONE;
    }
}
