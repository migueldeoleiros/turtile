#ifndef TURTILE_SERVER_H
#define TURTILE_SERVER_H

#include <wlr/types/wlr_xdg_shell.h>

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
    struct wl_listener new_xdg_surface;
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

void server_new_input(struct wl_listener *listener, void *data);

void server_new_xdg_surface(struct wl_listener *listener, void *data);

#endif // TURTILE_SERVER_H
