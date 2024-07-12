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

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>

#include "internal.h"
#include "framework.h"

struct fwiState frameworkState_s = {0};

struct fwiState* fwiGetState(void) {
    return &frameworkState_s;
}

void fwiLogA(const fwiLogLevel lll, const char* format_p,  ...) {
#ifdef BUILD_DEBUG
    const time_t rawTime  = time(nullptr);
    const struct tm* time = localtime(&rawTime);

    char buf[10] = {};
    const size_t bytesWritten = strftime(buf, 10, "[%H:%M:%S", time);
    if (bytesWritten == 0) {
        printf("Failed to get local time");
        return;
    }

    pthread_mutex_lock(&frameworkState_s.loggerMutex);

    switch (lll) {
        case fwiLogLevelError: {
            printf("%s "FW_ESCAPE_RED"ERROR"FW_ESCAPE_NORMAL"]: ", buf);
            break;
        }
        case fwiLogLevelWarning: {
            printf("%s "FW_ESCAPE_YELLOW"WARNING"FW_ESCAPE_NORMAL"]: ", buf);
            break;
        }
        case fwiLogLevelInfo: {
            printf("%s "FW_ESCAPE_CYAN"INFO"FW_ESCAPE_NORMAL"]: ", buf);
            break;
        }
        case fwiLogLevelDebug: {
            printf("%s "FW_ESCAPE_GREEN"DEBUG"FW_ESCAPE_NORMAL"]: ", buf);
            break;
        }
        case fwiLogLevelBench: {
            printf("%s "FW_ESCAPE_MAGENTA"BENCHMARK"FW_ESCAPE_NORMAL"]: ", buf);
            break;
        }
    }

    va_list args = {0u};
    va_start(args);
    vprintf(format_p, args);
    va_end(args);
    printf("\n");

    pthread_mutex_unlock(&frameworkState_s.loggerMutex);
#endif // BUILD_DEBUG
#ifdef BUILD_RELEASE
    // TODO: implement logging to file
#endif // BUILD_RELEASE
}

void fwiLogW(const fwiLogLevel lll, const wchar_t* format_p, ...) {
#ifdef BUILD_DEBUG
    const time_t rawTime  = time(nullptr);
    const struct tm* time = localtime(&rawTime);

    char buf[10]              = {};
    const size_t bytesWritten = strftime(buf, 30, "[%H:%M:%S", time);
    if (bytesWritten == 0) {
        printf("Failed to get local time");
        return;
    }

    pthread_mutex_lock(&frameworkState_s.loggerMutex);

    switch (lll) {
        case fwiLogLevelError: {
            printf("%s ERROR]: ", buf);
            break;
        }
        case fwiLogLevelWarning: {
            printf("%s WARNING]: ", buf);
            break;
        }
        case fwiLogLevelInfo: {
            printf("%s INFO]: ", buf);
            break;
        }
        case fwiLogLevelDebug: {
            printf("%s DEBUG]: ", buf);
            break;
        }
        case fwiLogLevelBench: {
            printf("%s BENCHMARK]: ", buf);
            break;
        }
    }

    va_list args = {0u};
    va_start(args);
    vwprintf(format_p, args);
    va_end(args);
    printf("\n");

    pthread_mutex_unlock(&frameworkState_s.loggerMutex);
#endif // BUILD_DEBUG
#ifdef BUILD_RELEASE
    // TODO: implement logging to file
#endif // BUILD_RELEASE
}

void fwiLogFollowupA(const bool isLast, const char* format_p, ...) {
#ifdef BUILD_DEBUG
    va_list args = {0u};
    va_start(args);
    if (isLast) {
        printf("\\ - ");
        vprintf(format_p, args);
        printf("\n");
    }
    else {
        printf("| - ");
        vprintf(format_p, args);
        printf("\n");
    }
    va_end(args);
#endif // BUILD_DEBUG
#ifdef BUILD_RELEASE
    //TODO: implement follow-up logging to file
#endif // BUILD_RELEASE
}

void fwiLogFollowupW(const bool isLast, const wchar_t* format_p, ...) {
#ifdef BUILD_DEBUG
    va_list args = {0u};
    va_start(args);
    if (isLast) {
        wprintf(L"\\ - ");
        vwprintf(format_p, args);
        wprintf(L"\n");
    }
    else {
        wprintf(L"| - ");
        vwprintf(format_p, args);
        wprintf(L"\n");
    }
    va_end(args);
#endif // BUILD_DEBUG
#ifdef BUILD_RELEASE
    //TODO: implement follow-up logging to file
#endif // BUILD_RELEASE
}

void fwiStartNativeModuleBase(void) {
    pthread_mutex_init(&frameworkState_s.loggerMutex, nullptr);

    const time_t rawTime        = time(nullptr);
    const struct tm* time       = localtime(&rawTime);
    char buf[14]                = {};
    const size_t bytesWritten   = strftime(buf, 14, "%d.%m.%Y", time);
    if (bytesWritten == 0) {
        fwiLogA(fwiLogLevelError, "Failed to get local time");
    }

    frameworkState_s.activeModules |= fwModuleBase;

    fwiLogA(fwiLogLevelInfo, "Base module was started");
    fwiLogA(fwiLogLevelInfo, "The current date is %s (D.M.Y)", buf);
}

void fwiStopNativeModuleBase(void) {
    frameworkState_s.activeModules &= ~fwModuleBase;
    fwiLogA(fwiLogLevelInfo, "Base module was stopped");
    pthread_mutex_destroy(&frameworkState_s.loggerMutex);
}
