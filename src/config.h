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

#ifndef TURTILE_CONFIG_H
#define TURTILE_CONFIG_H

#include <xkbcommon/xkbcommon.h>
#include <wayland-util.h> 
#include <libconfig.h>

typedef struct keybind {
    xkb_keysym_t keys[3];
    char *cmd;
    struct wl_list link;
} turtile_keybind_t;

typedef struct autostart {
    char *cmd;
    struct wl_list link;
} turtile_autostart_t;

typedef struct config {
    struct wl_list keybinds;
    struct wl_list autostart;
} turtile_config_t;

typedef struct {
    const char *name;
    void (*load)(config_t *cfg, const char *value);
} config_param_t;

void load_keybinds(config_t *cfg, const char *value);
void load_autostart(config_t *cfg, const char *value);

/**
 * Returns the singleton instance of the configuration.
 *
 * @return The global configuration instance
 */
turtile_config_t *config_get_instance(void);

/**
 * Free the global configuration instance.
 * Releases any resources held by the configuration instance.
 */
void config_free_instance(void);

/**
 * Reads the configuration from a file and updates the global
 * configuration instance.
 *
 * @param filename The file path to load the configuration from
 */
void config_load_from_file(const char *filepath);

#endif // TURTILE_CONFIG_H
