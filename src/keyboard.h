#ifndef TURTILE_KEYBOARD_H
#define TURTILE_KEYBOARD_H

#include "server.h"

struct turtile_keyboard {
	struct wl_list link;
	struct turtile_server *server;
	struct wlr_keyboard *wlr_keyboard;

	struct wl_listener modifiers;
	struct wl_listener key;
	struct wl_listener destroy;
};

void keyboard_handle_modifiers(struct wl_listener *listener, void *data);

bool handle_keybinding(struct turtile_server *server, xkb_keysym_t sym);

void keyboard_handle_key(struct wl_listener *listener, void *data);

void keyboard_handle_destroy(struct wl_listener *listener, void *data);

void server_new_keyboard(struct turtile_server *server,
								struct wlr_input_device *device);
#endif // TURTILE_KEYBOARD_H
