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
    uint8_t activeModules;
    pthread_mutex_t loggerMutex;
} __attribute__((aligned(16)));
struct fwiState frameworkState_s = {0};

void fwiStartNativeModuleBase(
        void
        );

void fwiStartNativeModuleWindow(
        void
        );

void fwiStartNativeModuleNetwork(
        void
        );

void fwiStartNativeModuleMultimedia(
        void
        );

void fwiStartNativeModuleRenderer(
        void
        );

void fwiStopNativeModuleBase(
        void
        );

void fwiStopNativeModuleWindow(
        void
        );

void fwiStopNativeModuleNetwork(
        void
        );

void fwiStopNativeModuleMultimedia(
        void
        );

void fwiStopNativeModuleRenderer(
        void
        );

/**
 * @brief printf with added timestamp and log level color
 * @param lll[in] Log level of the message
 * @param format_p[in] Format string of the output message, mechanically the same as printf
 * @param ...[in] Replacement parameters for placeholders
 */
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
 */
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
 */
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
 */
void fwiLogFollowupW(
        bool isLast,
        const wchar_t* format_p,
        ...
        );

void fwiCrashToFatalError();

#endif //LPAF_INTERNAL_H
