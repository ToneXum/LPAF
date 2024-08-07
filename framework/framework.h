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

// This header contains all end-dev symbols

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
    fwErrorModule /*! The module failed to start */,

    fwErrorFileUnableToOpen /*! Unable to open or correctly open the requested file */,
    fwErrorFileStats /*! Failed to retrieve file information */,

    fwErrorSocketAddressInUse /*! This local address is already being used by another socket */,
    fwErrorSocketTargetName /*! Failed to resolve host / domain name */,
    fwErrorSocketConnection /*! Failed to connect to the specified target */,
    fwErrorSocketSend /*! Failed to send data over the socket */,
    fwErrorSocketReceive /*! Failed to receive data over the socket */,
    fwErrorSocketBind /*! Failed to bind socket */,
    fwErrorSocketListen /*! Failed to put a socket into the listening state */,
    fwErrorSocketAccept /*! Failed to accept a new connection */,
    fwErrorSocketNotBound /*! Could not listen on the socket since it was not bound */,

    fwErrorWindowConnect /*! Could not connect to the wayland server */,

    fwErrorGoodJob /*! You somehow caused a theoretically impossible failure */
} fwError;

/**
 * @brief Identifiers of LPAF's modules.
 * @note Used as parameter for @c fwStartModuleInfo.
 */
typedef enum fwModule : uint8_t {
    fwModuleWindow      = 0b0000'0001 /*! Module for windowed UI */,
    fwModuleRender      = 0b0000'0010 /*! Module for the renderer */,
    fwModuleNetwork     = 0b0000'0100 /*! Module for networking and sockets*/,
    fwModuleMultimedia  = 0b0000'1000 /*! Module for multimedia like video and sound */
} fwModule;

typedef enum fwModuleFlags : uint32_t {
    /*! Test */
    fwModuleFlag        = 0b0000'0000'0000'0000'0000'0000'0000'0000
} fwModuleFlags;

/**
 * @brief Starts a module of the framework.
 * @param module[in] Which module is supposed to be started
 * @param flags[in] Flags for the module, these may modify the behaviour of it
 * @return @c fwErrorSuccess No error occured
 * @return @c fwErrorInvalidParameter The provided module was not valid
 */ // PlatIndepImp
fwError fwStartModule(
    fwModule module,
    uint32_t flags
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

typedef uintptr_t fwSocket;

/**
 * @brief Identifiers for address spaces.
 * @note Used as parameter for @c fwSocketCreateInfo.
 */
typedef enum fwSocketAddressFamily : uint8_t {
    fwSocketAddressFamilyIPv4 /*! IPv4 internet address space */,
    fwSocketAddressFamilyIPv6 /*! IPv6 internet address space */,
    fwSocketAddressFamilyLocal /*! Machine-local address space */, // TODO: implement local sockets
} fwSocketAddressFamily;

/**
 * @brief Specifies the protocol that a socket uses to transmit data.
 * @note Used as parameter for @c fwSocketCreateInfo.
 */
typedef enum fwSocketProtocol : uint8_t {
    fwSocketProtocolStream /*! Transmission Controll Protocol (TCP) */,
    fwSocketProtocolDatagram /*! User Datagram Protocol (UDP) */,
    fwSocketProtocolRaw /*! No protocol, data gets sent as is without headers or similar */
} fwSocketProtocol;

/**
 * @brief Creates a two-way using the information given.
 * @param sfdop_p[out] Identifier for the newly created socket
 * @param addressFamily[in] Address family the socket is operating on
 * @param protocol[in] Handshake and connection protocol the socket will be using
 * @return @c fwErrorSuccess No error occured
 * @return @c fwErrorInvalidParameter A value that does not correspond to an enumerated identifier
 *                                    was passed to @c sockCrtInf
 * @note See @c fwSocketAddressFamily for address families and @c fwSocketProtocol for protocols.
 */ // PlatDepImp
fwError fwSocketCreate(
    fwSocket* sfdop_p,
    enum fwSocketAddressFamily addressFamily,
    enum fwSocketProtocol protocol
    );

/**
 * All available addresses
 */
#define FW_SOCKET_ADDRESS_ANY "any"

/**
 * @brief Specifies the address of another machine or local socket
 * @param target_p Is an address, name or path to the peer
 * @param port_p Is the port number the peer was bound to
 * @note If the address family is local, the port number will be ignored since that contruct does
 *       not apply to local sockets.
 * @note Used as parameter for @c fwSocketConnect and @c fwSocketBind .
 */
typedef struct fwSocketAddress {
    const char* target_p;
    const char* port_p;
} fwSocketAddress;

/**
 * @brief Attemps to connect the socket using the given information. For connection-less
 *        protocols (like UDP and raw), calling this function does nothing.
 * @param sfdop[in] Identifier for the socket that is supposed to be connected
 * @param connectInfo_p[in] Information about where to connect the socket to
 * @return @c fwErrorSuccess No error occured
 * @return @c fwErrorSocketTargetName Could not resolve the name of the target to an IP address
 * @return @c fwErrorSocketConnection Could not connect to the target
 * @return @c fwErrorInvalidParameter Invalid enumerations in @c createInfo_p
 */ // PlatDepImp
fwError fwSocketConnect(
    fwSocket sfdop,
    const struct fwSocketAddress* connectInfo_p
    );

/**
 * @brief Binds a socket to a local interface and port number
 * @param sfdop[in] Socket that is supposed to be bound
 * @param localAddress[in] Address information on where to bind the socket to
 * @return @c fwErrorSuccess No error occured
 * @return @c fwErrorSocketBind Failed to bind the socket
 */ // PlatDepImp
fwError fwSocketBind(
    fwSocket sfdop,
    const struct fwSocketAddress* localAddress
    );

/**
 * @brief Waits for, and then accepts an incoming connection on a socket
 * @param sfdop[in] Socket that will be handeling the incomming connection
 * @param newSocket[out] New socket created from the connection
 * @param foreignAddress[out] Address of the peer that opened the connection
 * @return @c fwErrorSuccess No error occured
 * @return @c fwErrorOutOfMemory Out of memory
 * @return @c fwErrorSocketNotBound The socket that is supposed to handle the incomming
 *                                  connection was not bound
 * @return @c fwErrorSocketListen The system call, marking the socket as listening, failed
 * @return @c fwErrorSocketAccept Failed to accept the new connection
 * @note The reason why using one socket to accept the connection and then spawning another, is to
 *       re-use the old socket to accept more connections. Each new socket spawned is the connected
 *       state, such that it can immediatly be used to interact with the peer.
 * @note If the connection is unwanted, the new socket can be immediatly closed with
 *       @c fwSocketClose() .
 * @note The capacity of the @c foreignAddress output buffer must be at least 16 byte for an IPv4
 *       address, 46 byte for an IPv6 address and 108 byte for a local address.
 */ // PlatDepImp
fwError fwSocketAccept(
    fwSocket sfdop,
    fwSocket* newSocket,
    char* foreignAddress
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
 */ // PlatDepImp
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
 */ // PlatDepImp
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
 */ // PlatDepImp
fwError fwSocketClose(
    fwSocket sfdop
    );

//TODO: checkable socket connection status

#endif //LPAF_FRAMEWORK_H
