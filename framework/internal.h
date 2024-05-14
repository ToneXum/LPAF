#ifndef LPAF_INTERNAL_H
#define LPAF_INTERNAL_H

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
struct fwiStateData {
    uint8_t activeModules;
}; struct fwiStateData fwiState;

void fwiStartNativeModuleWindow();
void fwiStartNativeModuleNetwork();
void fwiStartNativeModuleMultimedia();
void fwiStartNativeModuleRenderer();

void fwiStopNativeModuleWindow();
void fwiStopNativeModuleNetwork();
void fwiStopNativeModuleMultimedia();
void fwiStopNativeModuleRenderer();

void fwiLogA(const char* msg, enum fwiLogLevel);
void fwiLogW(const wchar_t* msg, enum fwiLogLevel);

void fwiLogFollowupA(const char* msg, bool isLast);
void fwiLogFollowupW(const wchar_t* msg, bool isLast);

#endif //LPAF_INTERNAL_H
