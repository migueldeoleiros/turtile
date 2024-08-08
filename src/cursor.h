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

#ifndef TURTILE_CURSOR_H
#define TURTILE_CURSOR_H

#include "server.h"
#include <wlr/types/wlr_input_device.h>

// enum turtile_cursor_mode is declared over in server.h to avoid recursion

/**
 * This function is called when a new pointer input device is detected. It simply
 * attaches the device to the server's cursor for handling.
 *
 * @param server - The turtile_server instance.
 * @param device - The wlr_input_device instance.
 */
void server_new_pointer(struct turtile_server *server,
                               struct wlr_input_device *device);

/**
 * This function is called when a client requests to set a cursor image. It checks
 * if the requesting client has pointer focus, and if so, sets the cursor image
 * to the requested surface with the specified hotspot.
 *
 * @param listener - The wl_listener instance.
 * @param data - The wl_argument instance.
 */
void seat_request_cursor(struct wl_listener *listener, void *data);

/**
 * This function is called when a client requests to set the selection. It always
 * honors the request and sets the selection to the requested source and serial.
 *
 * @param listener - The wl_listener instance.
 * @param data - The wlr_seat_request_set_selection_event instance.
 */
void seat_request_set_selection(struct wl_listener *listener, void *data);

/**
 * This function resets the cursor mode to passthrough, which means that the cursor
 * will be handled by the underlying wlr_cursor instance. It also sets the
 * grabbed_toplevel pointer to NULL.
 *
 * @param server - The turtile_server instance.
 */
void reset_cursor_mode(struct turtile_server *server);

/**
 * This function moves the grabbed toplevel to the new position based on the
 * current cursor position and the position of the cursor when the grab was
 * initiated.
 *
 * @param server - The turtile_server instance.
 * @param time - The time in milliseconds.
 */
void process_cursor_move(struct turtile_server *server, uint32_t time);

/**
 * This function resizes the grabbed toplevel based on the current cursor position
 * and the position of the cursor when the grab was initiated. It also handles
 * moving the toplevel if the user is resizing from the top or left edges.
 *
 * @param server - The turtile_server instance.
 * @param time - The time in milliseconds.
 */
void process_cursor_resize(struct turtile_server *server, uint32_t time);

/**
 * This function handles pointer motion events. If the cursor mode is not
 * passthrough, it delegates to the appropriate function for moving or resizing
 * the grabbed toplevel. Otherwise, it finds the toplevel under the cursor and
 * sends pointer enter and motion events to the client with pointer focus.
 *
 * @param server - The turtile_server instance.
 * @param time - The time in milliseconds.
 */
void process_cursor_motion(struct turtile_server *server, uint32_t time);

/**
 * This function handles relative pointer motion events. It moves the cursor to the
 * new position based on the delta and time_msec fields of the event, and then
 * calls process_cursor_motion to handle the motion event.
 *
 * @param listener - The wl_listener instance.
 * @param data - The wlr_pointer_motion_event instance.
 */
void server_cursor_motion(struct wl_listener *listener, void *data);

/**
 * This function handles absolute pointer motion events. It warps the cursor to
 * the new position based on the x and y fields of the event, and then calls
 * process_cursor_motion to handle the motion event.
 *
 * @param listener - The wl_listener instance.
 * @param data - The wlr_pointer_motion_absolute_event instance.
 */
void server_cursor_motion_absolute(struct wl_listener *listener, void *data);

/**
 * This function handles pointer button events. If the button state is released,
 * it exits interactive move/resize mode. Otherwise, it focuses the toplevel
 * under the cursor.
 *
 * @param listener - The wl_listener instance.
 * @param data - The wlr_pointer_button_event instance.
 */
void server_cursor_button(struct wl_listener *listener, void *data);

/**
 * This function handles pointer axis events, like moving the scroll wheel.
 * It notifies the client with pointer focus of the axis event.
 *
 * @param listener - The wl_listener instance.
 * @paramn data - The wlr_pointer_axis_event instance.
 */
void server_cursor_axis(struct wl_listener *listener, void *data);

/**
 * This function handles pointer frame events.
 * Frame events are sent after regular pointer events to group
 * multiple events together. For instance, two axis events may happen at the
 * same time, in which case a frame event won't be sent in between.
 * It notifies the client with pointer focus of the frame event.
 *
 * @param listener - The wl_listener instance.
 * @param data - The wlr_pointer_frame_event instance.
 */
void server_cursor_frame(struct wl_listener *listener, void *data);

#endif // TURTILE_CURSOR_H
