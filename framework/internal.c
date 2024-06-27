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
#include <string.h>
#include <time.h>
#include <wchar.h>

#include "framework.h"
#include "internal.h"

struct fwiState* fwiGetState() {
    static struct fwiState dat = {};
    return &dat;
}

void fwiLogA(enum fwiLogLevel lll, const char* format,  ...) {
#ifdef BUILD_DEBUG
    time_t rawTime = time(nullptr);
    struct tm* time = localtime(&rawTime);

    char buf[10] = {};
    size_t bytesWritten = strftime(buf, 10, "[%H:%M:%S", time);
    if (bytesWritten == 0) {
        printf("Failed to get local time");
        return;
    }

    pthread_mutex_lock(&fwiGetState()->loggerMutex);

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
    vprintf(format, args);
    va_end(args);
    printf("\n");

    pthread_mutex_unlock(&fwiGetState()->loggerMutex);
#endif // BUILD_DEBUG
#ifdef BUILD_RELEASE
    // TODO: implement logging to file for ASCI
#endif // BUILD_RELEASE
}

void fwiLogW(enum fwiLogLevel lll, const wchar_t* format, ...) {
#ifdef BUILD_DEBUG
    time_t rawTime = time(nullptr);
    struct tm* time = localtime(&rawTime);

    char buf[10] = {};
    size_t bytesWritten = strftime(buf, 30, "[%H:%M:%S", time);
    if (bytesWritten == 0) {
        printf("Failed to get local time");
        return;
    }

    pthread_mutex_lock(&fwiGetState()->loggerMutex);

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
    vwprintf(format, args);
    va_end(args);
    printf("\n");

    pthread_mutex_unlock(&fwiGetState()->loggerMutex);
#endif // BUILD_DEBUG
#ifdef BUILD_RELEASE
    // TODO: implement logging to file for UFT-16
#endif // BUILD_RELEASE
}

void fwiLogFollowupA(bool isLast, const char* format, ...) {
#ifdef BUILD_DEBUG
    va_list args = {0u};
    va_start(args);
    if (isLast) {
        printf("\\ - ");
        vprintf(format, args);
        printf("\n");
    }
    else {
        printf("| - ");
        vprintf(format, args);
        printf("\n");
    }
    va_end(args);
#endif // BUILD_DEBUG
#ifdef BUILD_RELEASE
    //TODO: implement follow-up logging to file for ASCI
#endif // BUILD_RELEASE
}

void fwiLogFollowupW(bool isLast, const wchar_t* format, ...) {
#ifdef BUILD_DEBUG
    va_list args = {0u};
    va_start(args);
    if (isLast) {
        wprintf(L"\\ - ");
        vwprintf(format, args);
        wprintf(L"\n");
    }
    else {
        wprintf(L"| - ");
        vwprintf(format, args);
        wprintf(L"\n");
    }
    va_end(args);
#endif // BUILD_DEBUG
#ifdef BUILD_RELEASE
    //TODO: implement follow-up logging to file for UFT-16
#endif // BUILD_RELEASE
}

void fwiStartNativeModuleBase(void) {
    pthread_mutex_init(&fwiGetState()->loggerMutex, nullptr);

    time_t rawTime = time(nullptr);
    struct tm* time = localtime(&rawTime);

    char buf[14] = {};
    size_t bytesWritten = strftime(buf, 14, "%d.%m.%Y", time);
    if (bytesWritten == 0) {
        fwiLogA(fwiLogLevelError, "Failed to get local time");
    }

    fwiGetState()->activeModules |= fwModuleBase;

    fwiLogA(fwiLogLevelInfo, "Base module was started");
    fwiLogA(fwiLogLevelInfo, "The current date is %s (D.M.Y)", buf);
}

void fwiStopNativeModuleBase(void) {
    pthread_mutex_destroy(&fwiGetState()->loggerMutex);

    fwiGetState()->activeModules &= ~fwModuleBase;

    fwiLogA(fwiLogLevelInfo, "Base module was stopped");
}
