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
#include <stddef.h>

/**
 * @brief Error codes
 */
typedef enum fwError : uint8_t {
    fwErrorSuccess /*! No error, everything went smoothly */,
    fwErrorOutOfMemory /*! Out of memory, allocation failed */,
    fwErrorInvalidParameter /*! A parameter contained an illegal or wrong value */,
    fwErrorUnimplemented /*! This feature is not implemented on this platform */,

    fwErrorFileUnableToOpen /*! Unable to open or correctly open the requested file */,
    fwErrorFileStats /*! Failed to retrieve file information */,

    fwErrorSocketAddressInUse,
    fwErrorSocketDomainName,
    fwErrorSocketConnection,
    fwErrorSocketSend,
    fwErrorSocketReceive,

    fwErrorGoodJob /*! You somehow caused a theoretically impossible failure */
} fwError;

/**
 * @brief Identifies a module of LPAF
 * @note Used as parameter for @c fwStartModuleInfo
 */
typedef enum fwModule : uint8_t {
    fwModuleBase        = 0b0000'0001 /*! Base, passing this will cause error but no UB */,
    fwModuleWindow      = 0b0000'0010 /*! Module for windowed UI */,
    fwModuleRender      = 0b0000'0100 /*! Module for the renderer */,
    fwModuleNetwork     = 0b0000'1000 /*! Module for networking and sockets*/,
    fwModuleMultimedia  = 0b0001'0000 /*! Module for multimedia like video and sound */
} fwModule;

/**
 * @brief The information used to start a module
 * @param module @c fwModule enum specifying which module is supposed to be started
 * @note Used as parameter for @c fwStartModule()
 */
typedef struct fwStartModuleInfo {
    enum fwModule module;
    // uint8_t moduleStartFlags; TODO: implement module context
} fwStartInfo;

/**
 * @brief Starts a module of the framework
 * @param startInfo_kp[in] Pointer to a @c fwStartModuleInfo struct containing information about the
 *                        module that is supposed to be started
 * @return @c fwErrorSuccess No error occured
 * @return @c fwErrorInvalidParameter The provided module was not valid
 */ // PlatIndepImp
fwError fwStartModule(
        const struct fwStartModuleInfo* startInfo_kp
        );

/**
 * @brief Stops a module of the framework
 * @note See @c fwModule enum for modules
 * @param module[in] Which module is supposed to be stopped
 * @return @c fwErrorSuccess No error occured
 * @return @c fwErrorInvalidParameter The module parameter was not a valid module
 */ // PlatIndepImp
fwError fwStopModule(
        enum fwModule module
        );

/**
 * @brief Stops all currently active modules
 */ // PlatIndepImp
void fwStopAllModules(
        void
        );

/**
 * @brief Struct containing system configuration information
 * @param memory Physical memory in @b MebbiByte
 * @param cores Online cores per socket
 * @param sockets Sockets with processors installed
 * @note Used as param for @c fwGetSystemConfiguration
 */
typedef struct fwSystemConfiguration {
    uint64_t memory;
    uint16_t cores;
    uint8_t sockets;
} fwSystemConfiguration;

/**
 * @brief Retrieves the system configuration
 * @note TODO: This is implementation is shit, rework this.
 */ // PlatDepImp
fwError fwGetSystemConfiguration(
    struct fwSystemConfiguration* res_p
    );

/**
 * @brief Loads an entire file into an allocated memory buffer
 * @note This function will allocate using malloc, when the buffer becomes unused it should be
 *       released with @c free()
 * @param filename_p[in] Name of, or path to, the file
 * @param buffer_pp[out] Address of a pointer to the buffer in which the file will be stored, will
 *                       be allocted by this function
 * @param fileSize_p[out] Size of the file and the buffer in bytes
 * @return @c fwErrorSuccess No error occured
 * @return @c fwErrorCouldNotOpenFile The file could not be opened, due to either permissions
 *         or the file not existing
 * @return @c fwErrorOutOfMemory The file does not fit into free memory
 * @return @c fwErrorFailedToGetFileStats An I/O error occurs at syscall
 */ // PlatDepImp
fwError fwLoadFileToMem(
    const char* filename_p,
    void** buffer_pp,
    uint64_t* fileSize_p
);

typedef uintptr_t fwSocket;

typedef enum fwSocketAddressFamily : uint8_t {
    fwSocketAddressFamilyIPv4,
    fwSocketAddressFamilyIPv6,
    fwSocketAddressFamilyLocal,
    fwSocketAddressFamilyBlueTooth
} fwSocketAddressFamily;

typedef enum fwSocketType : uint8_t {
    fwSocketTypeStream,
    fwSocketTypeDatagram,
    fwSocketTypeRaw
} fwSocketType;

typedef struct fwSocketCreateInfo {
    enum fwSocketAddressFamily addressFamily;
    enum fwSocketType socketType;
} fwSocketCreateInfo;

fwError fwSocketCreate(
    const struct fwSocketCreateInfo* sockCrtInf,
    fwSocket* sfdop_p
    );

typedef enum fwSocketConnectionTargetKind : uint8_t {
    fwSocketConnectionTargetInternetProtocollAddress,
    fwSocketConnectionTargetInternetDomainName,
    fwSocketConnectionTargetLocalHostNamme
} fwSocketConnectionTarget;

typedef struct fwSocketConnectInfo {
    const char* target_p;
    const char* port_p;
    enum fwSocketConnectionTargetKind targetKind;
} fwSocketConnectionInfo;

fwError fwSocketConnect(
    fwSocket sfdop,
    const struct fwSocketCreateInfo* createInfo_p,
    const struct fwSocketConnectInfo* connectInfo_p
    );

fwError fwSocketSend(
    fwSocket sfdop,
    const void* data,
    size_t ammount
    );

fwError fwSocketReceive(
    fwSocket sfdop,
    void* buffer,
    size_t ammount
);

fwError fwSocketClose(
    fwSocket sfdop
    );

#endif //LPAF_FRAMEWORK_H
