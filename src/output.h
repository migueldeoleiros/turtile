#ifndef TURTILE_OUTPUT_H
#define TURTILE_OUTPUT_H

#include "server.h"

struct turtile_output {
    struct wl_list link;
    struct turtile_server *server;
    struct wlr_output *wlr_output;
    struct wl_listener frame;
    struct wl_listener request_state;
    struct wl_listener destroy;
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
