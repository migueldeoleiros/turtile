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

#ifndef TURTILE_SERVER_H
#define TURTILE_SERVER_H

#include <wayland-server-core.h>
#include <wlroots-0.18/wlr/util/box.h>

enum turtile_cursor_mode {
    TURTILE_CURSOR_PASSTHROUGH,
    TURTILE_CURSOR_MOVE,
    TURTILE_CURSOR_RESIZE,
};

struct turtile_server {
    struct wl_display *wl_display;
    struct wlr_backend *backend;
    struct wlr_renderer *renderer;
    struct wlr_allocator *allocator;
    struct wlr_scene *scene;
    struct wlr_scene_output_layout *scene_layout;

    struct wlr_xdg_shell *xdg_shell;
    struct wl_listener new_xdg_toplevel;
    struct wl_listener new_xdg_popup;
    struct wl_list toplevels;

    struct wlr_cursor *cursor;
    struct wlr_xcursor_manager *cursor_mgr;
    struct wl_listener cursor_motion;
    struct wl_listener cursor_motion_absolute;
    struct wl_listener cursor_button;
    struct wl_listener cursor_axis;
    struct wl_listener cursor_frame;

    struct wlr_seat *seat;
    struct wl_listener new_input;
    struct wl_listener request_cursor;
    struct wl_listener request_set_selection;
    struct wl_list keyboards;
    enum turtile_cursor_mode cursor_mode;
    struct turtile_toplevel *grabbed_toplevel;
    double grab_x, grab_y;
    struct wlr_box grab_geobox;
    uint32_t resize_edges;

    struct wlr_output_layout *output_layout;
    struct wl_list outputs;
    struct wl_listener new_output;
};

/**
 * This function is called when a new input device becomes available. It handles
 * the event by determining the type of the device and calling the appropriate
 * function to handle it. It also sets the capabilities of the wlr_seat object
 * based on the available input devices.
 *
 * @param listener - The listener that received the event.
 * @param data - The input device that was created.
 */
void server_new_input(struct wl_listener *listener, void *data);

// TODO update comments
void server_new_xdg_toplevel(struct wl_listener *listener, void *data);

void server_new_xdg_popup(struct wl_listener *listener, void *data);

#endif // TURTILE_SERVER_H
