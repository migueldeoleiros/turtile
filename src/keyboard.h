/* ----------------------------------------------------------------------------
   turtile - Simple Wayland compositor based on wlroots 
   Copyright (C) 2024  Miguel López López

   This file is part of turtile.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, see
   <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------------
*/

#ifndef TURTILE_KEYBOARD_H
#define TURTILE_KEYBOARD_H

#include "server.h"
#include <wlr/types/wlr_input_device.h>
#include <xkbcommon/xkbcommon.h>

struct turtile_keyboard {
    struct wl_list link;
    struct turtile_server *server;
    struct wlr_keyboard *wlr_keyboard;

    struct wl_listener modifiers;
    struct wl_listener key;
    struct wl_listener destroy;
};

/**
 * This function is called when a modifier key, such as shift or alt, is
 * pressed. It sets the current keyboard as the seat's keyboard and sends the
 * modifiers to the client.
 *
 * @param listener - The wl_listener structure associated with the modifiers
 *                   event.
 * @param data - A void pointer to the data associated with the event.
 */
void keyboard_handle_modifiers(struct wl_listener *listener, void *data);

/**
 * This function handles compositor keybindings. This is when the compositor
 * is processing keys, rather than passing them on to the client.
 *
 * @param server - The turtile_server structure representing the compositor.
 * @param modifier - The modifier keys pressed 
 * @param sym - The xkb_keysym_t value of the key that was pressed.
 *
 * @returns A boolean indicating whether the keybinding was handled or not.
 */
bool handle_keybinding(struct turtile_server *server, uint32_t modifiers,
					   xkb_keysym_t sym);

/**
 * This function is called when a key is pressed or released. It first checks
 * if the key is a compositor keybinding. If it is, it processes the
 * keybinding. Otherwise, it passes the key event along to the client.
 *
 * @param listener - The wl_listener structure associated with the key event.
 * @param data - A void pointer to the data associated with the event.
 */
void keyboard_handle_key(struct wl_listener *listener, void *data);

/**
 * This function is called when the wlr_keyboard is destroyed. It removes the
 * listeners associated with the keyboard and frees the memory used by the
 * turtile_keyboard structure.
 *
 * @param listener - The wl_listener structure associated with the destroy event.
 * @param data - A void pointer to the data associated with the event.
 */
void keyboard_handle_destroy(struct wl_listener *listener, void *data);


/**
 * This function is called when a new keyboard is detected. It sets up the
 * necessary listeners for keyboard events, sets the keymap and repeat rate,
 * and adds the keyboard to the list of keyboards.
 *
 * @param server - The turtile_server structure representing the compositor.
 * @param device - The wlr_input_device structure associated with the keyboard.
 */
void server_new_keyboard(struct turtile_server *server,
                                struct wlr_input_device *device);
#endif // TURTILE_KEYBOARD_H
