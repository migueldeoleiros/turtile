#ifndef TURTILE_TOPLEVEL_H
#define TURTILE_TOPLEVEL_H

#include "cursor.h"

struct turtile_toplevel {
	struct wl_list link;
	struct turtile_server *server;
	struct wlr_xdg_toplevel *xdg_toplevel;
	struct wlr_scene_tree *scene_tree;
	struct wl_listener map;
	struct wl_listener unmap;
	struct wl_listener destroy;
	struct wl_listener request_move;
	struct wl_listener request_resize;
	struct wl_listener request_maximize;
	struct wl_listener request_fullscreen;
};

void focus_toplevel(struct turtile_toplevel *toplevel,
						   struct wlr_surface *surface);

struct turtile_toplevel *desktop_toplevel_at(
		struct turtile_server *server, double lx, double ly,
		struct wlr_surface **surface, double *sx, double *sy);

void xdg_toplevel_map(struct wl_listener *listener, void *data);

void xdg_toplevel_unmap(struct wl_listener *listener, void *data);

void xdg_toplevel_destroy(struct wl_listener *listener, void *data);

void begin_interactive(struct turtile_toplevel *toplevel,
							  enum turtile_cursor_mode mode, uint32_t edges);

void xdg_toplevel_request_move(struct wl_listener *listener, void *data);

void xdg_toplevel_request_resize(struct wl_listener *listener, void *data);

void xdg_toplevel_request_maximize(struct wl_listener *listener, void *data);

void xdg_toplevel_request_fullscreen(struct wl_listener *listener, void *data);

#endif // TURTILE_TOPLEVEL_H
