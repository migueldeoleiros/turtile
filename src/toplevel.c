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

#include "toplevel.h"
#include "src/server.h"
#include "src/workspace.h"
#include <stdlib.h>
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_xcursor_manager.h>

void focus_toplevel(struct turtile_toplevel *toplevel, struct wlr_surface *surface) {
    /* Note: this function only deals with keyboard focus. */
    if (toplevel == NULL) {
        return;
    }
    struct turtile_server *server = toplevel->server;
    struct wlr_seat *seat = server->seat;
    struct wlr_surface *prev_surface = seat->keyboard_state.focused_surface;
    if (prev_surface == surface) {
        /* Don't re-focus an already focused surface. */
        return;
    }
    if (prev_surface) {
        /*
         * Deactivate the previously focused surface. This lets the client know
         * it no longer has focus and the client will repaint accordingly, e.g.
         * stop displaying a caret.
         */
        struct wlr_xdg_toplevel *prev_toplevel =
            wlr_xdg_toplevel_try_from_wlr_surface(prev_surface);
        if (prev_toplevel != NULL) {
            wlr_xdg_toplevel_set_activated(prev_toplevel, false);
        }
    }
    struct wlr_keyboard *keyboard = wlr_seat_get_keyboard(seat);
	// switch to the right workspace
	/* switch_workspace(toplevel->workspace); */
	server->active_workspace = toplevel->workspace;
    /* Move the toplevel to the front */
	wl_list_remove(&toplevel->flink);
	wl_list_insert(&server->focus_toplevels, &toplevel->flink);
    /* Activate the new surface */
    wlr_xdg_toplevel_set_activated(toplevel->xdg_toplevel, true);
    /*
     * Tell the seat to have the keyboard enter this surface. wlroots will keep
     * track of this and automatically send key events to the appropriate
     * clients without additional work on your part.
     */
    if (keyboard != NULL) {
        wlr_seat_keyboard_notify_enter(seat, toplevel->xdg_toplevel->base->surface,
            keyboard->keycodes, keyboard->num_keycodes, &keyboard->modifiers);
    }
	server_redraw_windows(server);
}

struct turtile_toplevel *get_first_toplevel(struct turtile_server *server) {
	struct turtile_toplevel *toplevel;
	wl_list_for_each(toplevel, &server->focus_toplevels, flink)
		if (toplevel->workspace == server->active_workspace)
			return toplevel;
	return NULL;
}

struct turtile_toplevel *desktop_toplevel_at(
        struct turtile_server *server, double lx, double ly,
        struct wlr_surface **surface, double *sx, double *sy) {
    /* This returns the topmost node in the scene at the given layout coords.
     * We only care about surface nodes as we are specifically looking for a
     * surface in the surface tree of a turtile_toplevel. */
    struct wlr_scene_node *node = wlr_scene_node_at(
        &server->scene->tree.node, lx, ly, sx, sy);
    if (node == NULL || node->type != WLR_SCENE_NODE_BUFFER) {
        return NULL;
    }
    struct wlr_scene_buffer *scene_buffer = wlr_scene_buffer_from_node(node);
    struct wlr_scene_surface *scene_surface =
        wlr_scene_surface_try_from_buffer(scene_buffer);
    if (!scene_surface) {
        return NULL;
    }

    *surface = scene_surface->surface;
    /* Find the node corresponding to the turtile_toplevel at the root of this
     * surface tree, it is the only one for which we set the data field. */
    struct wlr_scene_tree *tree = node->parent;
    while (tree != NULL && tree->node.data == NULL) {
        tree = tree->node.parent;
    }
    return tree->node.data;
}

void toplevel_resize(
        struct turtile_toplevel *toplevel, struct wlr_box geometry) {
	toplevel->geometry = geometry;

	wlr_scene_node_set_position(&toplevel->scene_tree->node,
								toplevel->geometry.x, toplevel->geometry.y);
	wlr_xdg_toplevel_set_size(toplevel->xdg_toplevel, toplevel->geometry.width,
							  toplevel->geometry.height);
}

void xdg_toplevel_map(struct wl_listener *listener, void *data) {
    /* Called when the surface is mapped, or ready to display on-screen. */
    struct turtile_toplevel *toplevel = wl_container_of(listener, toplevel, map);

	toplevel->workspace = toplevel->server->active_workspace;
    wl_list_insert(&toplevel->server->toplevels, &toplevel->link);
    wl_list_insert(&toplevel->server->focus_toplevels, &toplevel->flink);

    focus_toplevel(toplevel, toplevel->xdg_toplevel->base->surface);
}

void xdg_toplevel_unmap(struct wl_listener *listener, void *data) {
    /* Called when the surface is unmapped, and should no longer be shown. */
    struct turtile_toplevel *toplevel = wl_container_of(listener, toplevel, unmap);

    /* Reset the cursor mode if the grabbed toplevel was unmapped. */
    if (toplevel == toplevel->server->grabbed_toplevel) {
        reset_cursor_mode(toplevel->server);

		struct turtile_toplevel *newfocus = get_first_toplevel(toplevel->server);
		focus_toplevel(newfocus, newfocus->xdg_toplevel->base->surface);
    }

    wl_list_remove(&toplevel->link);
}

void xdg_toplevel_commit(struct wl_listener *listener, void *data) {
    /* Called when a new surface state is committed. */
    struct turtile_toplevel *toplevel = wl_container_of(listener, toplevel, commit);

    if (toplevel->xdg_toplevel->base->initial_commit) {
        /* When an xdg_surface performs an initial commit, the compositor must
         * reply with a configure so the client can map the surface. turtile
         * configures the xdg_toplevel with 0,0 size to let the client pick the
         * dimensions itself. */
        wlr_xdg_toplevel_set_size(toplevel->xdg_toplevel, 0, 0);
    }
}

void xdg_toplevel_destroy(struct wl_listener *listener, void *data) {
    /* Called when the xdg_toplevel is destroyed. */
    struct turtile_toplevel *toplevel = wl_container_of(listener, toplevel, destroy);

    wl_list_remove(&toplevel->map.link);
    wl_list_remove(&toplevel->unmap.link);
    wl_list_remove(&toplevel->commit.link);
    wl_list_remove(&toplevel->destroy.link);
    wl_list_remove(&toplevel->request_move.link);
    wl_list_remove(&toplevel->request_resize.link);
    wl_list_remove(&toplevel->request_maximize.link);
    wl_list_remove(&toplevel->request_fullscreen.link);

	server_redraw_windows(toplevel->server);
    free(toplevel);
}

void begin_interactive(struct turtile_toplevel *toplevel,
        enum turtile_cursor_mode mode, uint32_t edges) {
    /* This function sets up an interactive move or resize operation, where the
     * compositor stops propagating pointer events to clients and instead
     * consumes them itself, to move or resize windows. */
    struct turtile_server *server = toplevel->server;
    struct wlr_surface *focused_surface =
        server->seat->pointer_state.focused_surface;
    if (toplevel->xdg_toplevel->base->surface !=
            wlr_surface_get_root_surface(focused_surface)) {
        /* Deny move/resize requests from unfocused clients. */
        return;
    }
    server->grabbed_toplevel = toplevel;
    server->cursor_mode = mode;

    if (mode == TURTILE_CURSOR_MOVE) {
        server->grab_x = server->cursor->x - toplevel->scene_tree->node.x;
        server->grab_y = server->cursor->y - toplevel->scene_tree->node.y;
    } else {
        struct wlr_box geo_box;
        wlr_xdg_surface_get_geometry(toplevel->xdg_toplevel->base, &geo_box);

        double border_x = (toplevel->scene_tree->node.x + geo_box.x) +
            ((edges & WLR_EDGE_RIGHT) ? geo_box.width : 0);
        double border_y = (toplevel->scene_tree->node.y + geo_box.y) +
            ((edges & WLR_EDGE_BOTTOM) ? geo_box.height : 0);
        server->grab_x = server->cursor->x - border_x;
        server->grab_y = server->cursor->y - border_y;

        server->grab_geobox = geo_box;
        server->grab_geobox.x += toplevel->scene_tree->node.x;
        server->grab_geobox.y += toplevel->scene_tree->node.y;

        server->resize_edges = edges;
    }
}

void xdg_toplevel_request_move(
        struct wl_listener *listener, void *data) {
    /* This event is raised when a client would like to begin an interactive
     * move, typically because the user clicked on their client-side
     * decorations. Note that a more sophisticated compositor should check the
     * provided serial against a list of button press serials sent to this
     * client, to prevent the client from requesting this whenever they want. */
    struct turtile_toplevel *toplevel = wl_container_of(listener, toplevel, request_move);
    begin_interactive(toplevel, TURTILE_CURSOR_MOVE, 0);
}

void xdg_toplevel_request_resize(
        struct wl_listener *listener, void *data) {
    /* This event is raised when a client would like to begin an interactive
     * resize, typically because the user clicked on their client-side
     * decorations. Note that a more sophisticated compositor should check the
     * provided serial against a list of button press serials sent to this
     * client, to prevent the client from requesting this whenever they want. */
    struct wlr_xdg_toplevel_resize_event *event = data;
    struct turtile_toplevel *toplevel = wl_container_of(listener, toplevel, request_resize);
    begin_interactive(toplevel, TURTILE_CURSOR_RESIZE, event->edges);
}

void xdg_toplevel_request_maximize(
        struct wl_listener *listener, void *data) {
    /* This event is raised when a client would like to maximize itself,
     * typically because the user clicked on the maximize button on
     * client-side decorations. turtile doesn't support maximization, but
     * to conform to xdg-shell protocol we still must send a configure.
     * wlr_xdg_surface_schedule_configure() is used to send an empty reply.
     * However, if the request was sent before an initial commit, we don't do
     * anything and let the client finish the initial surface setup. */
    struct turtile_toplevel *toplevel =
        wl_container_of(listener, toplevel, request_maximize);
    if (toplevel->xdg_toplevel->base->initialized) {
		wlr_xdg_surface_schedule_configure(toplevel->xdg_toplevel->base);
    }
}

void xdg_toplevel_request_fullscreen(
        struct wl_listener *listener, void *data) {
    /* Just as with request_maximize, we must send a configure here. */
    struct turtile_toplevel *toplevel =
        wl_container_of(listener, toplevel, request_fullscreen);
    if (toplevel->xdg_toplevel->base->initialized) {
        wlr_xdg_surface_schedule_configure(toplevel->xdg_toplevel->base);
    }
}
