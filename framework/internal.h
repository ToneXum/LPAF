#ifndef LPAF_INTERNAL_H
#define LPAF_INTERNAL_H

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
 * @param activeModules Bitfield indicating which modules are currently running
 * @see fwModule enum for module-specific bits
 */
struct fwiStateData {
    uint8_t activeModules;
}; struct fwiStateData fwiState;

/**
 * @brief Starts the windowing module used for UI
 */
void fwiStartNativeModuleWindow();

/**
 * @brief Starts the networking module used for networking and internet communication
 */
void fwiStartNativeModuleNetwork();

/**
 * @brief Starts the multimedia module used to handling sound and video media
 */
void fwiStartNativeModuleMultimedia();

/**
 * @brief Starts the renderer module used for graphical and visual computation
 */
void fwiStartNativeModuleRenderer();

/**
 * @brief Stops the windowing module
 */
void fwiStopNativeModuleWindow();

/**
 * @brief Stops the networking module
 */
void fwiStopNativeModuleNetwork();

/**
 * @brief Stops the multimedia module
 */
void fwiStopNativeModuleMultimedia();

/**
 * @brief Stops the renderer module
 */
void fwiStopNativeModuleRenderer();

void fwiFancyLogA(const char* msg, enum fwiLogLevel);
void fwiFancyLogW(const wchar_t* msg, enum fwiLogLevel);

void fwiFancyLogFollowupA(const char* msg, bool last);
void fwiFancyLogFollowupW(const wchar_t* msg, bool last);

#endif //LPAF_INTERNAL_H
