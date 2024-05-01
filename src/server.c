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

#include "server.h"

#include "cursor.h"
#include "keyboard.h"
#include "toplevel.h"
#include <assert.h>
#include <stdlib.h>
#include <wlr/types/wlr_scene.h>

void server_new_input(struct wl_listener *listener, void *data) {
    /* This event is raised by the backend when a new input device becomes
     * available. */
    struct turtile_server *server =
        wl_container_of(listener, server, new_input);
    struct wlr_input_device *device = data;
    switch (device->type) {
    case WLR_INPUT_DEVICE_KEYBOARD:
        server_new_keyboard(server, device);
        break;
    case WLR_INPUT_DEVICE_POINTER:
        server_new_pointer(server, device);
        break;
    default:
        break;
    }
    /* We need to let the wlr_seat know what our capabilities are, which is
     * communiciated to the client. In Turtile we always have a cursor, even if
     * there are no pointer devices, so we always include that capability. */
    uint32_t caps = WL_SEAT_CAPABILITY_POINTER;
    if (!wl_list_empty(&server->keyboards)) {
        caps |= WL_SEAT_CAPABILITY_KEYBOARD;
    }
    wlr_seat_set_capabilities(server->seat, caps);
}

void server_new_xdg_surface(struct wl_listener *listener, void *data) {
    /* This event is raised when wlr_xdg_shell receives a new xdg surface from a
     * client, either a toplevel (application window) or popup. */
    struct turtile_server *server =
        wl_container_of(listener, server, new_xdg_surface);
    struct wlr_xdg_surface *xdg_surface = data;

    /* We must add xdg popups to the scene graph so they get rendered. The
     * wlroots scene graph provides a helper for this, but to use it we must
     * provide the proper parent scene node of the xdg popup. To enable this,
     * we always set the user data field of xdg_surfaces to the corresponding
     * scene node. */
    if (xdg_surface->role == WLR_XDG_SURFACE_ROLE_POPUP) {
        struct wlr_xdg_surface *parent =
            wlr_xdg_surface_try_from_wlr_surface(xdg_surface->popup->parent);
        assert(parent != NULL);
        struct wlr_scene_tree *parent_tree = parent->data;
        xdg_surface->data = wlr_scene_xdg_surface_create(
            parent_tree, xdg_surface);
        return;
    }
    assert(xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL);

    /* Allocate a turtile_toplevel for this surface */
    struct turtile_toplevel *toplevel = calloc(1, sizeof(*toplevel));
    toplevel->server = server;
    toplevel->xdg_toplevel = xdg_surface->toplevel;
    toplevel->scene_tree = wlr_scene_xdg_surface_create(
            &toplevel->server->scene->tree, toplevel->xdg_toplevel->base);
    toplevel->scene_tree->node.data = toplevel;
    xdg_surface->data = toplevel->scene_tree;

    /* Listen to the various events it can emit */
    toplevel->map.notify = xdg_toplevel_map;
    wl_signal_add(&xdg_surface->surface->events.map, &toplevel->map);
    toplevel->unmap.notify = xdg_toplevel_unmap;
    wl_signal_add(&xdg_surface->surface->events.unmap, &toplevel->unmap);
    toplevel->destroy.notify = xdg_toplevel_destroy;
    wl_signal_add(&xdg_surface->events.destroy, &toplevel->destroy);

    /* cotd */
    struct wlr_xdg_toplevel *xdg_toplevel = xdg_surface->toplevel;
    toplevel->request_move.notify = xdg_toplevel_request_move;
    wl_signal_add(&xdg_toplevel->events.request_move, &toplevel->request_move);
    toplevel->request_resize.notify = xdg_toplevel_request_resize;
    wl_signal_add(&xdg_toplevel->events.request_resize, &toplevel->request_resize);
    toplevel->request_maximize.notify = xdg_toplevel_request_maximize;
    wl_signal_add(&xdg_toplevel->events.request_maximize,
        &toplevel->request_maximize);
    toplevel->request_fullscreen.notify = xdg_toplevel_request_fullscreen;
    wl_signal_add(&xdg_toplevel->events.request_fullscreen,
        &toplevel->request_fullscreen);
}
