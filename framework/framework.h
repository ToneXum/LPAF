#ifndef LPAF_FRAMEWORK_H
#define LPAF_FRAMEWORK_H

#include <stdint.h>

/**
 * @brief Specifies a module of LPAF. For example the renderer or the networking subsystem
 * @see Used as parameter for @c fwStartModuleInfo
 */
typedef enum fwModule : uint8_t {
    fwModuleBase        = 0b00001 /*! Base */,
    fwModuleWindow      = 0b00010 /*! Module for windowed UI */,
    fwModuleRender      = 0b00100 /*! Module for the renderer */,
    fwModuleNetwork     = 0b01000 /*! Module for networking and sockets*/,
    fwModuleMultimedia  = 0b10000 /*! Module for multimedia like video and sound */
} lfModule;

/**
 * @brief Flags that specify how a module should be started; They modify the behaviour of said
 * module
 */
typedef enum fwModuleStartFlags : uint8_t {
    fwWindowModuleStartNoWindowClose          = 0b10
} lfModuleStartFlags;

/**
 * @brief Error codes returned by various functions when failure occurs
 */
typedef enum fwError : uint8_t {
    fwErrorOutOfMemory /*! Out of memory, allocation failed */,
    fwErrorInvalidParameter /*! A parameter contained an illegal or wrong value */
} fwError;

/**
 * @brief Enum identifying event callbacks
 * @see Used for @c fwSetEventCallback() as parameter
 */
typedef enum fwEventCallback {
    fwEventCallbackMouseMove /*! The mouse has moved*/,
    fwEventCallbackKeyDown /*! A key was pressed */ ,
    fwEventCallbackKeyUp /*! A key was released */,
    fwEventCallbackCloseRequest /*! A Window was requested to be closed */,
    fwEventCallbackClose /*! A window was closed */,
} fwEventCallback;

/**
 * @brief The information used to start a module
 * @param[in] kModule Enum of type @c fwModule ; Specifies which module is supposed to be started
 * @param[in] kModuleStartFlags Flag set of type @c fwModuleStartFlags
 * @see @c fwModuleStartFlags for the aforementioned flags
 * @see Used as parameter for @c fwStartModule()
 */
typedef struct fwStartModuleInfo {
    enum fwModule module;
    uint8_t moduleStartFlags;
} __attribute__((aligned(2))) lfStartInfo;

/**
 * @brief Starts a module of the framework
 * @param[in] kpStartInfo Pointer a @c fwStartModuleInfo struct containing information about the
 * module that is supposed to be started
 * @return @c NULL on success, an error code on failure
 * @see @c fwStartModuleInfo for starting information
 * @see @c fwError enum for error codes
 */
fwError fwStartModule(
        const struct fwStartModuleInfo* kpStartInfo
        );

/**
 * @brief Stops a module of the framework
 * @param[in] module Which module is supposed to be stopped
 * @see @c fwModule enum for modules
 */
void fwStopModule(
        enum fwModule module
        );

void fwStopAllModules(
        void
        );

/**
 * @brief Sets the callback function for a certain event; The function will then be executed when
 * said event occurs
 * @param[in] event Which event to set the callback for
 * @param[int] func The function that will be executed once the event occurs
 * @see @c fwEventCallback enum for events
 */
fwError fwSetEventCallback(
        enum fwEventCallback event,
        uint16_t(*func)(void* high, void* low)
        );

#endif //LPAF_FRAMEWORK_H
