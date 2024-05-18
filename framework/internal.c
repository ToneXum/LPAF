#include <stdio.h>
#include <time.h>

#include "internal.h"

struct fwiState* fwiGetState() {
    static struct fwiState dat = {};
    return &dat;
}

void fwiLogA(const char* msg, enum fwiLogLevel lll) {
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

    printf("%s", msg);

    pthread_mutex_unlock(&fwiGetState()->loggerMutex);
#endif // BUILD_DEBUG
#ifdef BUILD_RELEASE
    // TODO: implement logging to file for ASCI
#endif // BUILD_RELEASE
}

void fwiLogW(const wchar_t* msg, enum fwiLogLevel lll) {
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

    wprintf(L"%s", msg);

    pthread_mutex_unlock(&fwiGetState()->loggerMutex);
#endif // BUILD_DEBUG
#ifdef BUILD_RELEASE
    // TODO: implement logging to file for UFT-16
#endif // BUILD_RELEASE
}

void fwiLogFollowupA(const char* msg, bool isLast) {
#ifdef BUILD_DEBUG
    if (isLast) {
        printf("\\ - %s", msg);
    }
    else {
        printf("| - %s", msg);
    }
#endif // BUILD_DEBUG
#ifdef BUILD_RELEASE
    //TODO: implement follow-up logging to file for ASCI
#endif // BUILD_RELEASE
}

void fwiLogFollowupW(const wchar_t* msg, bool isLast) {
#ifdef BUILD_DEBUG
    if (isLast) {
        wprintf(L"\\ - %s", msg);
    }
    else {
        wprintf(L"| - %s", msg);
    }
#endif // BUILD_DEBUG
#ifdef BUILD_RELEASE
    //TODO: implement follow-up logging to file for UFT-16
#endif // BUILD_RELEASE
}

void fwiStartNativeModuleBase(void) {
    pthread_mutex_init(&fwiGetState()->loggerMutex, nullptr);

    fwiLogA("Base module was started", fwiLogLevelInfo);
}

void fwiStopNativeModuleBase(void) {
    pthread_mutex_destroy(&fwiGetState()->loggerMutex);

    fwiLogA("Base module was stopped", fwiLogLevelInfo);
}
