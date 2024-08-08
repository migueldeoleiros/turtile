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

#include "config.h"
#include "wlr/util/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libconfig.h>

// Global configuration instance
static turtile_config_t *config_instance = NULL;

// Array of configuration parameters
config_param_t config_params[] = {
    {"keybinds", load_keybinds},
    {"autostart", load_autostart},
    // Add more configuration parameters here
};

// Helper function to create a new keybind
static turtile_keybind_t *keybind_create(xkb_keysym_t keys[3], const char *cmd) {
    turtile_keybind_t *keybind = malloc(sizeof(turtile_keybind_t));
    if (!keybind) {
		wlr_log(WLR_ERROR, "Failed to allocate keybind");
        return NULL;
    }
    keybind->keys[0] = keys[0];
    keybind->keys[1] = keys[1];
    keybind->keys[2] = keys[2];
    keybind->cmd = strdup(cmd);
    if (!keybind->cmd) {
        free(keybind);
        return NULL;
    }
    wl_list_init(&keybind->link);
    return keybind;
}

void load_keybinds(config_t *cfg, const char *value) {
    config_setting_t *keybinds_setting = config_lookup(cfg, "keybinds");
    if (!keybinds_setting) {
        wlr_log(WLR_ERROR, "Keybinds not found in configuration");
        return;
    }

    int count = config_setting_length(keybinds_setting);
    for (int i = 0; i < count; i++) {
        config_setting_t *keybind_setting = config_setting_get_elem(keybinds_setting, i);
        if (!keybind_setting) {
            continue;
        }

        const char *cmd;
        if (!config_setting_lookup_string(keybind_setting, "cmd", &cmd)) {
            wlr_log(WLR_ERROR, "Keybind missing command in configuration");
            continue;
        }

        config_setting_t *key_setting = config_setting_lookup(keybind_setting, "key");
        if (!key_setting || config_setting_length(key_setting) != 3) {
            wlr_log(WLR_ERROR, "Keybind has an invalid key array");
            continue;
        }

        xkb_keysym_t keys[3];
        for (int j = 0; j < 3; j++) {
            const char *key_str = config_setting_get_string_elem(key_setting, j);
            if (!key_str) {
                wlr_log(WLR_ERROR, "Keybind has an invalid key string");
                continue;
            }

            // Convert key string to keysym
            xkb_keysym_t keysym = xkb_keysym_from_name(key_str, XKB_KEYSYM_CASE_INSENSITIVE);
            if (keysym == XKB_KEY_NoSymbol) {
                wlr_log(WLR_ERROR, "Invalid key name '%s' in configuration", key_str);
                continue;
            }
            keys[j] = keysym;
        }

        turtile_keybind_t *keybind = keybind_create(keys, cmd);
        if (keybind) {
            wl_list_insert(&config_get_instance()->keybinds, &keybind->link);
        } else {
            wlr_log(WLR_ERROR, "Failed to create keybind");
        }
    }
}

// Helper function to create a new autostart
static turtile_autostart_t *autostart_create(const char *cmd) {
    turtile_autostart_t *autostart = malloc(sizeof(turtile_autostart_t));
    if (!autostart) {
		wlr_log(WLR_ERROR, "Failed to allocate autostart");
        return NULL;
    }
    autostart->cmd = strdup(cmd);
    if (!autostart->cmd) {
        free(autostart);
        return NULL;
    }
    wl_list_init(&autostart->link);
    return autostart;
}

void load_autostart(config_t *cfg, const char *value) {
    config_setting_t *autostart_setting = config_lookup(cfg, "autostart");
    if (!autostart_setting) {
        wlr_log(WLR_ERROR, "Autostart not found in configuration");
        return;
    }

    int count = config_setting_length(autostart_setting);
    for (int i = 0; i < count; i++) {
        const char *cmd = config_setting_get_string_elem(autostart_setting, i);
        if (!cmd) {
            wlr_log(WLR_ERROR, "Autostart command missing or invalid");
            continue;
        }

        turtile_autostart_t *autostart = autostart_create(cmd);
        if (autostart) {
            wl_list_insert(&config_get_instance()->autostart, &autostart->link);
        } else {
            wlr_log(WLR_ERROR, "Failed to create autostart command");
        }
    }
}

void config_load_from_file(const char *filepath) {
    config_t cfg;
    config_init(&cfg);
    if (!config_read_file(&cfg, filepath)) {
        wlr_log(WLR_ERROR, "Error reading configuration file: %s", filepath);
        config_destroy(&cfg);
        return;
    }

    // Iterate over the configuration parameters and load each one
    for (int i = 0; i < sizeof(config_params) / sizeof(config_params[0]); i++) {
        config_param_t *param = &config_params[i];
        config_setting_t *setting = config_lookup(&cfg, param->name);
        if (setting) {
            param->load(&cfg, config_setting_get_string(setting));
        }
    }

    config_destroy(&cfg);
}

turtile_config_t *config_get_instance(void) {
    if (!config_instance) {
        config_instance = malloc(sizeof(turtile_config_t));
        if (!config_instance) {
            return NULL;
        }
        wl_list_init(&config_instance->keybinds);
        wl_list_init(&config_instance->autostart);
    }
    return config_instance;
}

void config_free_instance(void) {
    if (config_instance) {
        // Free keybinds
        turtile_keybind_t *keybind, *tmp;
        wl_list_for_each_safe(keybind, tmp, &config_instance->keybinds, link) {
            free(keybind->cmd);
            free(keybind);
        }

        // Free autostart
        turtile_autostart_t *autostart, *tmp2;
        wl_list_for_each_safe(autostart, tmp2, &config_instance->autostart, link) {
            free(autostart->cmd);
            free(autostart);
        }

        free(config_instance);
        config_instance = NULL;
    }
}
