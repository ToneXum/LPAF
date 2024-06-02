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

typedef enum fwiLogLevel : uint8_t {
    fwiLogLevelError,
    fwiLogLevelWarning,
    fwiLogLevelInfo,
    fwiLogLevelDebug,
    fwiLogLevelBench /*! Runtime benchmarking */
} fwiLogLevel;

/**
 * @brief @b DO @b NOT @b INSTANTIATE; Global framework state data
 */
struct fwiState {
    uint8_t activeModules;
    pthread_mutex_t loggerMutex;
} __attribute__((aligned(16)));
struct fwiState* fwiGetState();

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

void fwiLogA(
        enum fwiLogLevel lll,
        const char* format,
        ...
        );

void fwiLogW(
        enum fwiLogLevel lll,
        const wchar_t* format,
        ...
        );

void fwiLogFollowupA(
        bool isLast,
        const char* format,
        ...
        );

void fwiLogFollowupW(
        bool isLast,
        const wchar_t* format,
        ...
        );

void fwiCrashToFatalError();

#endif //LPAF_INTERNAL_H
