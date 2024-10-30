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
#include "src/output.h"
#include "toplevel.h"
#include "popup.h"
#include "wlr/util/box.h"
#include "wlr/types/wlr_output_layout.h"
#include <assert.h>
#include <stdlib.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_xdg_shell.h>

void server_new_input(struct wl_listener *listener, void *data) {
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

void server_new_xdg_toplevel(struct wl_listener *listener, void *data) {
	struct turtile_server *server = wl_container_of(listener, server, new_xdg_toplevel);
	struct wlr_xdg_toplevel *xdg_toplevel = data;

	/* Allocate a turtile_toplevel for this surface */
	struct turtile_toplevel *toplevel = calloc(1, sizeof(*toplevel));
	toplevel->server = server;
	toplevel->xdg_toplevel = xdg_toplevel;
	toplevel->scene_tree =
		wlr_scene_xdg_surface_create(&toplevel->server->scene->tree, xdg_toplevel->base);
	toplevel->scene_tree->node.data = toplevel;
	xdg_toplevel->base->data = toplevel->scene_tree;

	/* Listen to the various events it can emit */
	toplevel->map.notify = xdg_toplevel_map;
	wl_signal_add(&xdg_toplevel->base->surface->events.map, &toplevel->map);
	toplevel->unmap.notify = xdg_toplevel_unmap;
	wl_signal_add(&xdg_toplevel->base->surface->events.unmap, &toplevel->unmap);
	toplevel->commit.notify = xdg_toplevel_commit;
	wl_signal_add(&xdg_toplevel->base->surface->events.commit, &toplevel->commit);

	toplevel->destroy.notify = xdg_toplevel_destroy;
	wl_signal_add(&xdg_toplevel->events.destroy, &toplevel->destroy);

	/* cotd */
	toplevel->request_move.notify = xdg_toplevel_request_move;
	wl_signal_add(&xdg_toplevel->events.request_move, &toplevel->request_move);
	toplevel->request_resize.notify = xdg_toplevel_request_resize;
	wl_signal_add(&xdg_toplevel->events.request_resize, &toplevel->request_resize);
	toplevel->request_maximize.notify = xdg_toplevel_request_maximize;
	wl_signal_add(&xdg_toplevel->events.request_maximize, &toplevel->request_maximize);
	toplevel->request_fullscreen.notify = xdg_toplevel_request_fullscreen;
	wl_signal_add(&xdg_toplevel->events.request_fullscreen, &toplevel->request_fullscreen);
}

void server_new_xdg_popup(struct wl_listener *listener, void *data) {
	struct wlr_xdg_popup *xdg_popup = data;

	struct turtile_popup *popup = calloc(1, sizeof(*popup));
	popup->xdg_popup = xdg_popup;

	/* We must add xdg popups to the scene graph so they get rendered. The
	 * wlroots scene graph provides a helper for this, but to use it we must
	 * provide the proper parent scene node of the xdg popup. To enable this,
	 * we always set the user data field of xdg_surfaces to the corresponding
	 * scene node. */
	struct wlr_xdg_surface *parent = wlr_xdg_surface_try_from_wlr_surface(xdg_popup->parent);
	assert(parent != NULL);
	struct wlr_scene_tree *parent_tree = parent->data;
	xdg_popup->base->data = wlr_scene_xdg_surface_create(parent_tree, xdg_popup->base);

	popup->commit.notify = xdg_popup_commit;
	wl_signal_add(&xdg_popup->base->surface->events.commit, &popup->commit);

	popup->destroy.notify = xdg_popup_destroy;
	wl_signal_add(&xdg_popup->events.destroy, &popup->destroy);
}

void tile(struct turtile_server *server) {
	int nmaster = 1; //number of master windows
	double mfact = 0.5; //master size
	unsigned int i, n = 0, mw, my, ty;
	struct turtile_toplevel *toplevel;

	struct wlr_box m;
	struct turtile_output *output;
	wl_list_for_each(output, &server->outputs, link)
		wlr_output_layout_get_box(server->output_layout, output->wlr_output, &m);

	wl_list_for_each(toplevel, &server->toplevels, link) {
		if (toplevel->workspace == server->active_workspace)
			n++;
	}
	if (n == 0)
		return;

	if (n > nmaster)
		mw = nmaster ? m.width * mfact : 0;
	else
		mw = m.width;
	i = my = ty = 0;
	wl_list_for_each(toplevel, &server->toplevels, link) {
		if (toplevel->workspace != server->active_workspace)
			continue;
		if (i < 1) {
			toplevel_resize(toplevel, (struct wlr_box){
							.x = m.x, .y = m.y + my,
							.width = mw,
							.height = (m.height - my) /
							(((n) < (nmaster) ? (n) : (nmaster)) - i)});
			my += toplevel->geometry.height;
		} else {
			toplevel_resize(toplevel, (struct wlr_box){
					.x = m.x + mw, .y = m.y + ty,
					.width = m.width - mw,
					.height = (m.height - ty) / (n - i)});
			ty += toplevel->geometry.height;
		}
		i++;
	}
}

void server_redraw_windows(struct turtile_server *server) {
	struct turtile_toplevel *toplevel;
	wl_list_for_each(toplevel, &server->toplevels, link) {
		if (toplevel->workspace == server->active_workspace) {
			wlr_scene_node_set_enabled(&toplevel->scene_tree->node, true);
		} else {
			wlr_scene_node_set_enabled(&toplevel->scene_tree->node, false);
		}

	}
	tile(server);
}
