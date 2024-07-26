// LPAF - lightweight and performant application framework
// Copyright (C) 2024 ToneXum (Toni Stein)
//
// This program is free software: you can redistribute it and/or modify it under the terms of the
// GNU General Public License as published by the Free Software Foundation, either version 3 of
// the License, or any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program. If
// not, see <https://www.gnu.org/licenses/>.

#ifndef LPAF_INTERNAL_H
#define LPAF_INTERNAL_H

#include <pthread.h>
#include <stdint.h>
#include <wchar.h>

/**
 * @brief ANSI escape sequences
 */
#define FW_ESCAPE_NORMAL    "\x1B[0m"
#define FW_ESCAPE_RED       "\x1B[31m"
#define FW_ESCAPE_GREEN     "\x1B[32m"
#define FW_ESCAPE_YELLOW    "\x1B[33m"
#define FW_ESCAPE_BLUE      "\x1B[34m"
#define FW_ESCAPE_MAGENTA   "\x1B[35m"
#define FW_ESCAPE_CYAN      "\x1B[36m"
#define FW_ESCAPE_WHITE     "\x1B[37m"

#define FW_ESCAPE_UP(x)     "\x1B["x"A"
#define FW_ESCAPE_DOWN(x)   "\x1B["x"B"
#define FW_ESCAPE_RIGHT(x)  "\x1B["x"C"
#define FW_ESCAPE_LEFT(x)   "\x1B["x"D"

#define FW_ESCAPE_CLEAR     "\x1B[2J" // clear entire screen

typedef enum fwiLogLevel : uint8_t {
    fwiLogLevelError,
    fwiLogLevelWarning,
    fwiLogLevelInfo,
    fwiLogLevelDebug,
    fwiLogLevelBench /*! Runtime benchmarking */
} fwiLogLevel;

/**
 * @brief Do not instanciate
 */
struct fwiState {
    pthread_mutex_t loggerMutex;
    uint8_t activeModules;
    bool baseIsUp;
};

struct fwiState* fwiGetState(
    void
    );

// PlatIndepImp
void fwiStartNativeModuleBase(
    void
    );

// PlatDepImp
void fwiStartNativeModuleWindow(
    void
    );

// PlatDepImp
void fwiStartNativeModuleNetwork(
    void
    );

// PlatDepImp
void fwiStartNativeModuleMultimedia(
    void
    );

// PlatDepImp
void fwiStartNativeModuleRenderer(
    void
    );

// PlatIndepImp
void fwiStopNativeModuleBase(
    void
    );

// PlatDepImp
void fwiStopNativeModuleWindow(
    void
    );

// PlatDepImp
void fwiStopNativeModuleNetwork(
    void
    );

// PlatDepImp
void fwiStopNativeModuleMultimedia(
    void
    );

// PlatDepImp
void fwiStopNativeModuleRenderer(
    void
    );

/**
 * @brief printf with added timestamp and log level color
 * @param lll[in] Log level of the message
 * @param format_p[in] Format string of the output message, mechanically the same as printf
 * @param ...[in] Replacement parameters for placeholders
 */ // PlatIndepImp
void fwiLogA(
    enum fwiLogLevel lll,
    const char* format_p,
    ...
    );

/**
 * @brief printf with added timestamp and log level color
 * @param lll[in] Log level of the message
 * @param format_p[in] Format string of the output message, placeholders are mechanically the same
 *                     as printf
 * @param ...[in] Replacement parameters for placeholders
 */ // PlatIndepImp
void fwiLogW(
    enum fwiLogLevel lll,
    const wchar_t* format_p,
    ...
    );

/**
 * @brief makes it look like the message was appended to the one before it
 * @param isLast[in] If this message is the last one, for visual completeness
 * @param format_p[in] Format string of the output message, placeholders are mechanically the same
 *                     as printf
 * @param ...[in] Replacement parameters for placeholders
 */ // PlatIndepImp
void fwiLogFollowupA(
    bool isLast,
    const char* format_p,
    ...
    );

/**
 * @brief makes it look like the message was appended to the one before it
 * @param isLast[in] If this message is the last one, for visual completeness
 * @param format_p[in] Format string of the output message, placeholders are mechanically the same
 *                     as printf
 * @param ...[in] Replacement parameters for placeholders
 */ // PlatIndepImp
void fwiLogFollowupW(
    bool isLast,
    const wchar_t* format_p,
    ...
    );

void fwiLogErrno(
    const char* location,
    int32_t line
    );



#endif //LPAF_INTERNAL_H
