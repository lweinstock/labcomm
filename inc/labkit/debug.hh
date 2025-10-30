#ifndef LC_DEBUG_HH
#define LC_DEBUG_HH

#include <cstdint>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <stdarg.h>

/*
 *  LC_DEBUG can be used instead of PRINT_DEBUG, but some
 *  code editors complain if LC_DEBUG is not defined
 */

#ifdef LC_DEBUG
    #define PRINT_DEBUG 1
#else
    #define PRINT_DEBUG 0
#endif

namespace labkit 
{

// Print message with additional information if LC_DEBUG is set
#define DEBUG_PRINT(msg, ...) \
    if (PRINT_DEBUG) debugPrint(stderr, __FILE__, __func__, msg, __VA_ARGS__)

// Print data string (max. 100 characters)
#define DEBUG_PRINT_STRING_DATA(data, msg, ...) \
    if (PRINT_DEBUG) { \
        debugPrint(stderr, __FILE__, __func__, msg, __VA_ARGS__); \
        debugPrintStringData(stderr, data); }

// Print data bytes (max. 60 characters)
#define DEBUG_PRINT_BYTE_DATA(data, len, msg, ...) \
    if (PRINT_DEBUG) { \
    debugPrint(stderr, __FILE__, __func__, msg, __VA_ARGS__); \
    debugPrintByteData(stderr, data, len); }

void debugPrint(FILE* stream, const char* file, const char* function, 
    const char* msg, ...);
void debugPrintStringData(FILE* stream, std::string data);
void debugPrintByteData(FILE* stream, const uint8_t* data, size_t len);

}

#endif