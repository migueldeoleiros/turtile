#ifndef TURTILE_CURSOR_H
#define TURTILE_CURSOR_H

#include "server.h"
#include <wlr/types/wlr_cursor.h>

// enum turtile_cursor_mode is declared over in server.h to avoid recursion

void server_new_pointer(struct turtile_server *server,
							   struct wlr_input_device *device);

void seat_request_cursor(struct wl_listener *listener, void *data);

void seat_request_set_selection(struct wl_listener *listener, void *data);

void reset_cursor_mode(struct turtile_server *server);

void process_cursor_move(struct turtile_server *server, uint32_t time);

void process_cursor_resize(struct turtile_server *server, uint32_t time);

void process_cursor_motion(struct turtile_server *server, uint32_t time);

void server_cursor_motion(struct wl_listener *listener, void *data);

void server_cursor_motion_absolute(struct wl_listener *listener, void *data);

void server_cursor_button(struct wl_listener *listener, void *data);

void server_cursor_axis(struct wl_listener *listener, void *data);

void server_cursor_frame(struct wl_listener *listener, void *data);

#endif // TURTILE_CURSOR_H
