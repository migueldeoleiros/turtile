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

#ifndef TURTILE_TOPLEVEL_H
#define TURTILE_TOPLEVEL_H

#include "cursor.h"
#include <wlr/types/wlr_xdg_shell.h>
#include <uuid/uuid.h>

struct turtile_toplevel {
    struct wl_list link;
    struct wl_list flink;
    struct wl_list auxlink;

	char id[9]; // 8 characters + null terminator
    struct turtile_server *server;
    struct wlr_xdg_toplevel *xdg_toplevel;
    struct wlr_scene_tree *scene_tree;
	struct turtile_workspace *workspace;
    struct wlr_box geometry;

    struct wl_listener map;
    struct wl_listener unmap;
    struct wl_listener commit;
    struct wl_listener destroy;
    struct wl_listener request_move;
    struct wl_listener request_resize;
    struct wl_listener request_maximize;
    struct wl_listener request_fullscreen;
};

/**
 * Focuses the given surface by setting it as the keyboard focused surface of
 * the compositor's seat. This function only deals with keyboard focus.
 *
 * @param toplevel - The turtile toplevel to focus the surface on.
 * @param surface - The surface to focus.
 */
void focus_toplevel(struct turtile_toplevel *toplevel,
                           struct wlr_surface *surface);
/**
 * Send close request to toplevel, and focus next window
 *
 * @param toplevel The turtile toplevel to kill.
 */
void kill_toplevel(struct turtile_toplevel *toplevel);

/**
 * Set given toplevel as master
 *
 * @param toplevel The turtile toplevel to set as master.
 */
void set_master_toplevel(struct turtile_toplevel *toplevel);
	
/**
 * Retrieves the toplevel with the given ID from the given server.
 *
 * @param server The turtile server to search for the toplevel on.
 * @param id The ID of the toplevel to retrieve.
 * @return A pointer to the toplevel with the given ID, or NULL
 */
struct turtile_toplevel *get_toplevel(struct turtile_server *server, char *id);

/**
 * Retrieves the first toplevel on the active workspace of the given server.
 * If no such toplevel is found, NULL is returned.

 * @param server The turtile server to search for the toplevel on.
 * @return A pointer to the first toplevel on the active workspace,
 *	       or NULL if no such toplevel is found.
 */
struct turtile_toplevel *get_first_toplevel(struct turtile_server *server);

/**
 * Retrieves the first toplevel of the focus stack on the active workspace
 * of the given server.
 * If no such toplevel is found, NULL is returned.

 * @param server The turtile server to search for the toplevel on.
 * @return A pointer to the first toplevel of the focus stack on the active
 *	       workspace, or NULL if no such toplevel is found.
 */
struct turtile_toplevel *get_first_focus_toplevel(struct turtile_server *server);

/**
 * Retrieves the next toplevel of the focus stack on the active workspace
 * of the given server.
 * If no such toplevel is found, NULL is returned.

 * @param server The turtile server to search for the toplevel on.
 * @return A pointer to the next toplevel of the focus stack on the active
 *	       workspace, or NULL if no such toplevel is found.
 */
struct turtile_toplevel *get_next_focus_toplevel(struct turtile_server *server);

/**
 * Given a server, layout coordinates, and optional surface and position
 * pointers, this function returns the topmost node in the scene at the given
 * layout coords. It only cares about surface nodes as we are specifically
 * looking for a surface in the surface tree of a turtile_toplevel.
 *
 * @param server - The turtile server to look for the surface in.
 * @param lx - The x coordinate of the layout position to look for.
 * @param ly - The y coordinate of the layout position to look for.
 * @param surface - An optional pointer to store the focused surface in.
 * @param sx - An optional pointer to store the x coordinate of the surface in.
 * @param sy - An optional pointer to store the y coordinate of the surface in.
 *
 * @returns A pointer to the topmost node in the scene at the given layout coords, or
 *  NULL if no such node is found.
 */
struct turtile_toplevel *desktop_toplevel_at(
        struct turtile_server *server, double lx, double ly,
        struct wlr_surface **surface, double *sx, double *sy);
/**
 * Resizes the given toplevel to the specified geometry.
 *
 * @param toplevel The turtile toplevel to resize.
 * @param geometry The new geometry for the toplevel.
 */
void toplevel_resize(
	struct turtile_toplevel *toplevel, struct wlr_box geometry);

/**
 * Called when the surface is mapped, or ready to display on-screen.
 *
 * @param listener - The listener that triggered this callback.
 * @param data - The data passed to the listener, which is the turtile toplevel
 *         associated with the surface.
 */
void xdg_toplevel_map(struct wl_listener *listener, void *data);

/**
 * Called when the surface is unmapped, and should no longer be shown.
 *
 * @param listener - The listener that triggered this callback.
 * @param data - The data passed to the listener, which is the turtile toplevel
 *         associated with the surface.
 */
void xdg_toplevel_unmap(struct wl_listener *listener, void *data);

/**
 * Called when a new surface state is committed.
 *
 * @param listener - The listener that triggered this callback.
 * @param data - The data passed to the listener, which is the turtile toplevel
 *         associated with the surface.
 */
void xdg_toplevel_commit(struct wl_listener *listener, void *data);

/**
 * Called when the xdg_toplevel is destroyed.
 *
 * @param listener - The listener that triggered this callback.
 * @param data - The data passed to the listener, which is the turtile toplevel
 *         associated with the surface.
 */
void xdg_toplevel_destroy(struct wl_listener *listener, void *data);

/**
 * Sets up an interactive move or resize operation, where the compositor
 * stops propagating pointer events to clients and instead consumes them
 * itself, to move or resize windows.
 *
 * @param toplevel - The turtile toplevel to move or resize.
 * @param mode - The mode of the interactive operation (TURTILE_CURSOR_MOVE or
 *         TURTILE_CURSOR_RESIZE).
 * @param edges - The edges to resize (for TURTILE_CURSOR_RESIZE mode).
 */
void begin_interactive(struct turtile_toplevel *toplevel,
                              enum turtile_cursor_mode mode, uint32_t edges);

/**
 * This event is raised when a client would like to begin an interactive move,
 * typically because the user clicked on their client-side decorations.
 *
 * @param listener - The listener that triggered this callback.
 * @param data - The data passed to the listener, which is the turtile toplevel
 *         associated with the surface.
 */
void xdg_toplevel_request_move(struct wl_listener *listener, void *data);

/**
 * This event is raised when a client would like to begin an interactive
 * resize, typically because the user clicked on their client-side
 * decorations.
 *
 * @param listener - The listener that triggered this callback.
 * @param data - The data passed to the listener, which is a
 *         wlr_xdg_toplevel_resize_event struct containing the resize edges.
 */
void xdg_toplevel_request_resize(struct wl_listener *listener, void *data);

/**
 * This event is raised when a client would like to maximize itself,
 * typically because the user clicked on the maximize button on
 * client-side decorations. turtile doesn't currently support maximization,
 * but to conform to xdg-shell protocol we still must send a configure.
 *
 * @param listener - The listener that triggered this callback.
 * @param data - The data passed to the listener, which is the turtile
 *         toplevel associated with the surface.
 */
void xdg_toplevel_request_maximize(struct wl_listener *listener, void *data);

/**
 * Just as with request_maximize, we must send a configure here.
 *
 * @param listener - The listener that triggered this callback.
 * @param data - The data passed to the listener, which is the turtile
 *         toplevel associated with the surface.
 */
void xdg_toplevel_request_fullscreen(struct wl_listener *listener, void *data);

#endif // TURTILE_TOPLEVEL_H
