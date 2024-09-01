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

#ifndef TURTILE_OUTPUT_H
#define TURTILE_OUTPUT_H

#include "server.h"

//forward declare workspace
struct turtile_workspace;

struct turtile_output {
    struct wl_list link;
    struct turtile_server *server;
    struct wlr_output *wlr_output;
    struct wl_listener frame;
    struct wl_listener request_state;
    struct wl_listener destroy;

	struct turtile_workspace *active_workspace; 
};

/**
 * This function is called every time an output is ready to display a frame,
 * generally at the output's refresh rate (e.g. 60Hz). It is responsible for
 * rendering the scene and committing the output.
 *
 * @param listener - The listener structure that triggered the callback.
 * @param data - A pointer to the output structure.
 */
void output_frame(struct wl_listener *listener, void *data);

/**
 * This function is called when the backend requests a new state for the
 * output. For example, Wayland and X11 backends request a new mode when the
 * output window is resized. It is responsible for committing the new state.
 *
 * @param listener -  The listener structure that triggered the callback.
 * @param data - A pointer to a const wlr_output_event_request_state structure
 *         containing the new state.
 */
void output_request_state(struct wl_listener *listener, void *data);

/**
 * This function is called when an output is destroyed. It is responsible for
 * cleaning up the output structure and removing it from any relevant lists.
 *
 * @param listener -  The listener structure that triggered the callback.
 * @param data - A pointer to the output structure.
 */
void output_destroy(struct wl_listener *listener, void *data);


/**
 * This event is raised by the backend when a new output (aka a display or
 * monitor) becomes available. It is responsible for initializing the output's
 * renderer, setting its state, and adding it to the output layout.
 *
 * @param listener - The listener structure that triggered the callback.
 * @param data - A pointer to the new wlr_output structure.
 */
void server_new_output(struct wl_listener *listener, void *data);

#endif // TURTILE_OUTPUT_H
