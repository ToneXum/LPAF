// LPAF - Lightweight and Performant Application Framework
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

// This is an exposed header
#ifndef LPAF_FRAMEWORK_H
#define LPAF_FRAMEWORK_H

#include <stdint.h>

// Only the finest documentation in here...

/**
 * @brief Error codes returned by various functions when failure occurs
 */
typedef enum fwError : uint8_t {
    fwErrorSuccess /*! No error, everything went smoothly */,
    fwErrorOutOfMemory /*! Out of memory, allocation failed */,
    fwErrorInvalidParameter /*! A parameter contained an illegal or wrong value */,
    fwErrorUnimplemented /*! This feature is not implemented on this platform */,
    fwErrorCouldNotOpenFile /*! Unable to open or correctly open the requested file */,
    fwErrorFailedToGetFileStats /*! Failed to retrieve file information */,
    fwErrorGoodJob /*! You somehow caused a theoretically impossible failure */
} fwError;

/**
 * @brief Specifies a module of LPAF. For example the renderer or the networking subsystem
 * @see Used as parameter for @c fwStartModuleInfo
 */
typedef enum fwModule : uint8_t {
    fwModuleBase        = 0b00001 /*! Base, passing this will cause error but no UB */,
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
fwError fwStopModule(
        enum fwModule module
        );

/**
 * @brief Stops all currently active modules
 */
void fwStopAllModules(
        void
        );

/**
 * @brief Struct containing system configuration information
 * @param memory Physical memory in @b MebbiByte
 * @param cores Online cores per socket
 * @param sockets Sockets with processors installed
 */
typedef struct fwSystemConfiguration {
    uint64_t memory;
    uint16_t cores;
    uint8_t sockets;

} fwSystemConfiguration;

/**
 * @brief Retrieves the system configuration
 * @param[out] res Pointer to struct where the result will be written to
 * @return Does not return an error... wait what?
 */
fwError fwGetSystemConfiguration(
    fwSystemConfiguration* res
    );

/**
 * @brief Loads an entire file into an allocated memory buffer
 * @note This function will allocate using malloc, when the buffer becomes unused it should be
 *       released with @c free()
 * @param[in] filename Name of, or path to, the file
 * @param[out] buffer Buffer in which the file will be stored, will be allocted by this function
 * @param[out] fileSize Size of the file and the buffer in bytes
 * @return @c fwErrorCouldNotOpenFile when the file could not be opened, due to either permissions
 *         or the file not existing
 * @return @c fwErrorOutOfMemory when the file does not fit into free memory
 * @return @c fwErrorFailedToGetFileStats when an I/O error occurs at syscall
 */
fwError fwLoadFileToMem(
    const char* filename,
    void** buffer,
    uint64_t* fileSize
);

#endif //LPAF_FRAMEWORK_H
