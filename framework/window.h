// LPAF - lightweight and performant application framework
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

#ifndef LPAF_WINDOW_H
#define LPAF_WINDOW_H

#include <stddef.h>

#include "framework.h"

typedef enum fwWindowStyle : uint8_t {
    fwWindowStyleNoClose        = 0b001,
    fwWindowStyleBorderless     = 0b010,
} fwWindowStyle;

typedef struct fwWindowCreateInfo {
    const wchar_t* name;
    uint16_t width, height;
    int16_t xPos, yPos;
} __attribute__((aligned(16))) fwWindowCreateInfo;

fwError fwWindowCreate(
        struct fwWindowCreateInfo* wndCrtInfo
);

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

#endif //LPAF_WINDOW_H
