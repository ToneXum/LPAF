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

#include <stddef.h>
#include <stdint.h>

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

    fwErrorSocketAddressInUse /*! This local address is already being used by another socket */,
    fwErrorSocketTargetName /*! Failed to resolve host / domain name */,
    fwErrorSocketConnection /*! Failed to connect to the specified target */,
    fwErrorSocketSend /*! Failed to send data over the socket */,
    fwErrorSocketReceive /*! Failed to receive data over the socket */,

    fwErrorGoodJob /*! You somehow caused a theoretically impossible failure */
} fwError;

/**
 * @brief Identifiers of LPAF's modules.
 * @note Used as parameter for @c fwStartModuleInfo.
 */
typedef enum fwModule : uint8_t {
    fwModuleBase        = 0b0000'0001 /*! Base, passing this will cause error but no UB */,
    fwModuleWindow      = 0b0000'0010 /*! Module for windowed UI */,
    fwModuleRender      = 0b0000'0100 /*! Module for the renderer */,
    fwModuleNetwork     = 0b0000'1000 /*! Module for networking and sockets*/,
    fwModuleMultimedia  = 0b0001'0000 /*! Module for multimedia like video and sound */
} fwModule;

/**
 * @brief The information used to start a module.
 * @param module @c fwModule enum specifying which module is supposed to be started
 * @note Used as parameter for @c fwStartModule().
 */
typedef struct fwStartModuleInfo {
    enum fwModule module;
    // uint8_t moduleStartFlags; TODO: implement module context
} fwStartInfo;

/**
 * @brief Starts a module of the framework.
 * @param startInfo_kp[in] Pointer to a @c fwStartModuleInfo struct containing information about the
 *                        module that is supposed to be started
 * @return @c fwErrorSuccess No error occured
 * @return @c fwErrorInvalidParameter The provided module was not valid
 */ // PlatIndepImp
fwError fwStartModule(
        const struct fwStartModuleInfo* startInfo_kp
        );

/**
 * @brief Stops a module of the framework.
 * @param module[in] Which module is supposed to be stopped
 * @return @c fwErrorSuccess No error occured
 * @return @c fwErrorInvalidParameter The module parameter was not a valid module
 * @note See @c fwModule enum for modules.
 */ // PlatIndepImp
fwError fwStopModule(
        enum fwModule module
        );

/**
 * @brief Stops all currently active modules.
 */ // PlatIndepImp
void fwStopAllModules(
        void
        );

/**
 * @brief Struct containing system configuration information.
 * @param memory Physical memory in @b MebbiByte
 * @param cores Online cores per socket
 * @param sockets Sockets with processors installed
 * @note Used as param for @c fwGetSystemConfiguration.
 */
typedef struct fwSystemConfiguration {
    uint64_t memory;
    uint16_t cores;
    uint8_t sockets;
} fwSystemConfiguration;

/**
 * @brief Retrieves the system configuration.
 * @note TODO: This is implementation is shit, rework this.
 */ // PlatDepImp
fwError fwGetSystemConfiguration(
    struct fwSystemConfiguration* res_p
    );

/**
 * @brief Loads an entire file into an allocated memory buffer.
 * @param filename_p[in] Name of, or path to, the file
 * @param buffer_pp[out] Address of a pointer to the buffer in which the file will be stored, will
 *                       be allocted by this function
 * @param fileSize_p[out] Size of the file and the buffer in bytes
 * @return @c fwErrorSuccess No error occured
 * @return @c fwErrorFileUnableToOpen The file could not be opened, due to either permissions
 *         or the file not existing
 * @return @c fwErrorOutOfMemory The file does not fit into free memory
 * @return @c fwErrorFileStats An I/O error occurs at syscall
 * @note This function will allocate using malloc, when the buffer becomes unused it should be
 *       released with @c free().
 */ // PlatDepImp
fwError fwLoadFileToMem(
    const char* filename_p,
    void** buffer_pp,
    uint64_t* fileSize_p
);

/**
 * @brief 8-byte number to respresent sockets. This is a file descriptor on Linux and a pointer to
 *        state data on Windows.
 */
typedef uintptr_t fwSocket;

/**
 * @brief Identifiers for address spaces.
 * @note Used as parameter for @c fwSocketCreateInfo.
 */
typedef enum fwSocketAddressFamily : uint8_t {
    fwSocketAddressFamilyIPv4 /*! IPv4 internet address space */,
    fwSocketAddressFamilyIPv6 /*! IPv6 internet address space */,
    fwSocketAddressFamilyLocal /*! Machine-local address space */,
    fwSocketAddressFamilyBlueTooth /*! BlueTooth address space */
} fwSocketAddressFamily;

/**
 * @brief Specifies the protocol that a socket uses to transmit data.
 * @note Used as parameter for @c fwSocketCreateInfo.
 */
typedef enum fwSocketProtocol : uint8_t {
    fwSocketTypeStream /*! Transmission Controll Protocol (TCP) */,
    fwSocketTypeDatagram /*! User Datagram Protocol (UDP) */,
    fwSocketTypeRaw /*! No protocol, data gets sent as is without headers or similar */
} fwSocketProtocol;

/**
 * @brief Provides information on what kind of socket is to be created.
 * @param addressFamily Enum that specifies the address family which the socket is supposed to
 *                      interact with.
 * @param socketProtocol Enum that specifies the protocol which the socket is supposed to use
 * @note Used as parameter for @c fwSocketCreate().
 * @note See @c fwSocketProtocol for supported protocols and @c fwSocketAddressFamily for address
 *       families.
 */
typedef struct fwSocketCreateInfo {
    enum fwSocketAddressFamily addressFamily;
    enum fwSocketProtocol socketProtocol;
} fwSocketCreateInfo;

/**
 * @brief Creates a two-way using the information given.
 * @param sockCrtInf[in] Socket creation information
 * @param sfdop_p[out] Identifier for the newly created socket
 * @return @c fwErrorSuccess No error occured
 * @return @c fwErrorInvalidParameter A value that does not correspond to an enumerated identifier
 *                                    was passed to @c sockCrtInf
 * @note See @c fwSocketCreateInfo for the sypnosis of creation information.
 */
fwError fwSocketCreate(
    const struct fwSocketCreateInfo* sockCrtInf, // TODO: inline members into here
    fwSocket* sfdop_p
    );

/**
 * @brief Specifies what @c target_p is.
 * @note Used as parameter in @c fwSocketConnectInfo.
 */
typedef enum fwSocketConnectionTargetKind : uint8_t {
    /*! The target is an IP address (like "127.0.0.1" or "0:0:0:1") */
    fwSocketConnectionTargetInternetProtocollAddress,
    /*! The target is a domain name (like "tonexum.org")*/
    fwSocketConnectionTargetInternetDomainName,
    /*! The target is a hostname for a machine on the local network (like "server" or "localhost") */
    fwSocketConnectionTargetLocalHostNamme
} fwSocketConnectionTargetKind;

/**
 * @brief Provides information on where to connect a socket to
 * @param target_p A null-terminated string of a host- or domain name or an IP address that matches
 *                 address space the socket was created for
 * @param port_p A null-terminated string of the @b foreign port, must only contain numerical digits
 * @param targetKind Enum as to what @c target_p actually is; Influences how the name is resolved
 * @note Used as parameter for @c fwSocketConnect .
 * @note See @c fwSocketConnectionTargetKind for kind of targets.
 */
typedef struct fwSocketConnectInfo {
    const char* target_p;
    const char* port_p;
    enum fwSocketConnectionTargetKind targetKind;
} fwSocketConnectInfo;

/**
 * @brief Attemps to connect the socket using the given information. For connection-less
 *        protocols (like UDP and raw), calling this function does nothing.
 * @param sfdop[in] Identifier for the socket that is supposed to be connected
 * @param createInfo_p[in] The same information the socket was created with
 * @param connectInfo_p[in] Information about where to connect the socket to
 * @return @c fwErrorSuccess No error occured
 * @return @c fwErrorSocketTargetName Could not resolve the name of the target to an IP address
 * @return @c fwErrorSocketConnection Could not connect to the target
 * @return @c fwErrorInvalidParameter Target IP was too long (>14 characters) or Invalid
 *                                    enumerations in @c createInfo_p
 * @note Missmatch or manipulation of the socket creation information may lead to failure.
 */
fwError fwSocketConnect(
    fwSocket sfdop,
    const struct fwSocketCreateInfo* createInfo_p,
    const struct fwSocketConnectInfo* connectInfo_p
    );

/**
 * @brief Sends data over a connected socket.
 * @param sfdop[in] Socket that is supposed to send the data
 * @param data[in] Buffer containing the data
 * @param ammount[in] Number of bytes that are supposed to be sent
 * @return @c fwErrorSuccess No error occured
 * @return @c fwErrorSocketSend Failed to send data
 * @note Passing an identifier to a socket that is not connected or one that operates over a
 *       connection-less protocol will cause failure.
 */
fwError fwSocketSend(
    fwSocket sfdop,
    const void* data,
    size_t ammount
    );

/**
 * @brief Receives data over a connected socket.
 * @param sfdop[in] Socket that is supposed the receive the data
 * @param buffer[in] Destination buffer
 * @param ammount[in] Maximum ammount of bytes the call is allowed to write to @c buffer
 * @return @c fwErrorSuccess No error occured
 * @return @c fwErrorSocketReceive Failed to receive data
 * @note Passing an identifier to a socket that is not connected or one that operates over a
 *       connection-less protocol will cause failure.
 */
fwError fwSocketReceive(
    fwSocket sfdop,
    void* buffer,
    size_t ammount
);

/**
 * @brief Closes the specified socket.
 * @param sfdop[in] Socket to be closed
 * @return @c fwErrorSuccess No error occured
 * @return @c fwErrorInvalidParameter The socket passed was not valid
 * @note Closing a socket wont make its locally bound address immediatly accessable again since the
 *       kernel still holds information about it. Only after approximatly 2 minutes, the data is
 *       released.
 */
fwError fwSocketClose(
    fwSocket sfdop
    );

#endif //LPAF_FRAMEWORK_H
