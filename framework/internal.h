#ifndef LPAF_INTERNAL_H
#define LPAF_INTERNAL_H

#include <pthread.h>
#include <stdint.h>

/**
 * @brief Enumeration for specifying the log level for the @c FancyLogX functions
 */
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
