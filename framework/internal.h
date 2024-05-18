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
        const char* msg,
        enum fwiLogLevel lll
        );

void fwiLogW(
        const wchar_t* msg,
        enum fwiLogLevel lll
        );

void fwiLogFollowupA(
        const char* msg,
        bool isLast
        );

void fwiLogFollowupW(
        const wchar_t* msg,
        bool isLast
        );

void fwiCrashToFatalError();

#endif //LPAF_INTERNAL_H
