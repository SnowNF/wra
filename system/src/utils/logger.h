//
// Created by SnowNF on 24-7-12.
//

#ifndef WRA_LOGGER_H
#define WRA_LOGGER_H

#include <cassert>
#include <cstdio>

typedef enum {
    WRA_P_ERROR = 0,
    WRA_P_WARNING,
    WRA_P_INFO,
    WRA_P_DEBUG,
    WRA_P_FATAL,
} WRA_LOG_LEVEL;

#define wra_print(level, msg, ...) do{ \
static const char *level_strings[] = { \
        /* the order is important */ \
        "error", \
        "warning", \
        "info", \
        "debug", \
        "fatal" \
}; \
 \
static const char *levels[5] = {"0;31", "0;33", "0;32", "0;36", "0;33"}; \
fprintf(stderr,"\033[%sm%s : " msg"\033[0m\n", levels[level], level_strings[level], ## __VA_ARGS__); \
}while(0)

#define wra_debug(format, ...) wra_print(WRA_P_DEBUG, format, ## __VA_ARGS__)
#define wra_info(format, ...) wra_print(WRA_P_INFO, format, ## __VA_ARGS__)
#define wra_warn(format, ...) wra_print(WRA_P_WARNING, format, ## __VA_ARGS__)
#define wra_error(format, ...) wra_print(WRA_P_ERROR, format, ## __VA_ARGS__)
#define wra_fatal(format, ...) do{wra_print(WRA_P_FATAL, format, ## __VA_ARGS__); assert(false);}while(0)

#endif //WRA_LOGGER_H
