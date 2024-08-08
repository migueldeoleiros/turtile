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

#ifndef TURTILE_POPUP_H
#define TURTILE_POPUP_H

#include <wayland-server-core.h>

struct turtile_popup {
	struct wlr_xdg_popup *xdg_popup;
    struct wl_listener commit;
    struct wl_listener destroy;
};

/**
 * Called when a new surface state is committed.
 *
 * @param listener - The listener that triggered this callback.
 * @param data - The data passed to the listener, which is the turtile
 *         popup associated with the surface.
 */
void xdg_popup_commit(struct wl_listener *listener, void *data);

/**
 * Called when the xdg_popup is destroyed.
 *
 * @param listener - The listener that triggered this callback.
 * @param data - The data passed to the listener, which is the turtile
 *         popup associated with the surface.
 */
void xdg_popup_destroy(struct wl_listener *listener, void *data);

#endif // TURTILE_POPUP_H
