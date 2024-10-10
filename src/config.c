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
#include "src/server.h"
#include "src/workspace.h"
#include "wlr/util/log.h"
#include "wlr/types/wlr_keyboard.h"
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
    {"workspaces", load_workspaces},
    // Add more configuration parameters here
};

// Helper function to create a new keybind
static turtile_keybind_t *keybind_create(uint32_t mods, xkb_keysym_t key, const char *cmd) {
    turtile_keybind_t *keybind = malloc(sizeof(turtile_keybind_t));
    if (!keybind) {
		wlr_log(WLR_ERROR, "Failed to allocate keybind");
        return NULL;
    }
    keybind->mods = mods;
    keybind->key = key;
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

		// Fetch and parse modifiers
        config_setting_t *mod_setting = config_setting_lookup(keybind_setting, "mod");
        uint32_t mods = 0;
        if (mod_setting) {
            int mod_count = config_setting_length(mod_setting);
            for (int j = 0; j < mod_count; j++) {
                const char *mod_str = config_setting_get_string_elem(mod_setting, j);
                if (!mod_str) {
                    wlr_log(WLR_ERROR, "Invalid modifier string in configuration");
                    continue;
                }

                // Parse modifier strings into bitmask
                if (strstr(mod_str, "shift")) {
                    mods |= WLR_MODIFIER_SHIFT;
                } else if (strstr(mod_str, "ctrl")) {
                    mods |= WLR_MODIFIER_CTRL;
                } else if (strstr(mod_str, "alt")) {
                    mods |= WLR_MODIFIER_ALT;
                } else if (strstr(mod_str, "mod4")) {
                    mods |= WLR_MODIFIER_LOGO;
                } else if (strstr(mod_str, "mod2")) {
                    mods |= WLR_MODIFIER_MOD2;
                } else {
                    wlr_log(WLR_ERROR, "Unknown modifier '%s' in configuration", mod_str);
                }
            }
        }

        const char *key_str;
        if (!config_setting_lookup_string(keybind_setting, "key", &key_str)) {
            wlr_log(WLR_ERROR, "Keybind missing command in configuration");
            continue;
        }
		if (!key_str) {
			wlr_log(WLR_ERROR, "Keybind has an invalid key string");
			continue;
		}
		
		// Convert key string to keysym
		xkb_keysym_t key = xkb_keysym_from_name(key_str, XKB_KEYSYM_CASE_INSENSITIVE);
		if (key == XKB_KEY_NoSymbol) {
			wlr_log(WLR_ERROR, "Invalid key name '%s' in configuration", key_str);
			continue;
		}

        const char *cmd;
        if (!config_setting_lookup_string(keybind_setting, "cmd", &cmd)) {
            wlr_log(WLR_ERROR, "Keybind missing command in configuration");
            continue;
        }

        turtile_keybind_t *keybind = keybind_create(mods, key, cmd);
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

// Helper function to create a new workspace
static turtile_workspace_config_t *workspace_config_create(const char *name) {
    turtile_workspace_config_t *workspace =
		malloc(sizeof(turtile_workspace_config_t));
    if (!workspace) {
		wlr_log(WLR_ERROR, "Failed to allocate workspace");
        return NULL;
    }
    workspace->name = strdup(name);
    if (!workspace->name) {
        free(workspace);
        return NULL;
    }
    wl_list_init(&workspace->link);
    return workspace;
}

void load_workspaces(config_t *cfg, const char *value) {
    config_setting_t *workspaces_setting = config_lookup(cfg, "workspaces");
    if (!workspaces_setting) {
        wlr_log(WLR_ERROR, "Workspaces not found in configuration");
        return;
    }

    int count = config_setting_length(workspaces_setting);
    for (int i = 0; i < count; i++) {
        const char *name = config_setting_get_string_elem(workspaces_setting, i);
        if (!name || strlen(name) > 100) {
            wlr_log(WLR_ERROR, "Workspace name missing or invalid");
            continue;
        }

        turtile_workspace_config_t *workspace = workspace_config_create(name);
        if (workspace) {
            wl_list_insert(&config_get_instance()->workspaces, &workspace->link);
        } else {
            wlr_log(WLR_ERROR, "Failed to create workspace config");
        }
    }
}

void config_load_from_file(const char *filepath) {
    char full_path[256];
    realpath(filepath, full_path);
    wlr_log(WLR_INFO, "Attempting to load config from: %s\n", full_path);

    config_t cfg;
    config_init(&cfg);
    if (!config_read_file(&cfg, filepath)) {
        wlr_log(WLR_ERROR, "Error reading configuration file: %s", filepath);
		wlr_log(WLR_ERROR, "Error on line %d: %s\n", config_error_line(&cfg),
				config_error_text(&cfg));
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
        wl_list_init(&config_instance->workspaces);
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

        // Free workspaces
        turtile_workspace_config_t *workspace, *tmp3;
        wl_list_for_each_safe(workspace, tmp3, &config_instance->workspaces, link) {
            free(workspace->name);
            free(workspace);
        }
        free(config_instance);
        config_instance = NULL;
    }
}
