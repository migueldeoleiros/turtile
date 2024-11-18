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
#include "commands.h"
#include "socket_server.h"
#include "src/server.h"
#include "src/toplevel.h"
#include "src/workspace.h"
#include "wlr/util/log.h"
#include <stdio.h>
#include <string.h>
#include <wayland-util.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <json-c/json.h>

// Declare functions so that they can be referenced in the list |commands|
void exit_command(char *tokens[], int ntokens, char *response,
					struct turtile_context *context);
void window_command(char *tokens[], int ntokens, char *response,
					struct turtile_context *context);
void window_list_command(char *tokens[], int ntokens, char *response,
						 struct turtile_context *context);
void window_switch_command(char *tokens[], int ntokens, char *response,
						   struct turtile_context *context);
void window_cycle_command(char *tokens[], int ntokens, char *response,
						  struct turtile_context *context);
void window_kill_command(char *tokens[], int ntokens, char *response,
						 struct turtile_context *context);
void window_move_to_command(char *tokens[], int ntokens, char *response,
							struct turtile_context *context);
void window_master_toggle_command(char *tokens[], int ntokens, char *response,
								  struct turtile_context *context);
void workspace_command(char *tokens[], int ntokens, char *response,
					   struct turtile_context *context);
void workspace_list_command(char *tokens[], int ntokens, char *response,
							struct turtile_context *context);
void workspace_switch_command(char *tokens[], int ntokens, char *response,
							  struct turtile_context *conntext);
typedef struct {
    char *cmd_name;
    char *subcmd_name;
    void (*cmd_fun)(char *tokens[], int ntokens, char *response,
                   struct turtile_context *context);
} command_t;

// List of commands with their associated functions 
static command_t commands[] = {
    {"exit", NULL, exit_command},
    {"window", "list", window_list_command},
    {"window", "switch", window_switch_command},
    {"window", "cycle", window_cycle_command},
    {"window", "kill", window_kill_command},
    {"window", "move-to", window_move_to_command},
    {"window", "mtoggle", window_master_toggle_command},
    {"window", NULL, window_command},
    {"workspace", "list", workspace_list_command},
    {"workspace", "switch", workspace_switch_command},
    {"workspace", NULL, workspace_command},
    {NULL, NULL, NULL} // Terminate array with NULLs
};

/**
 * Execute a subcommand with the given tokens and context.
 *
 * @param tokens       An array of tokens representing the subcommand and
 *                     its arguments.
 * @param ntokens      The number of tokens in the tokens array.
 * @param response     A buffer to store the response to the subcommand.
 * @param context      A pointer to the turtile context structure.
 * @param subcommands  A pointer to the subcommands array.
 */
void execute_subcommand(char *tokens[], int ntokens, char *response,
                        struct turtile_context *context,
						command_t *subcommands);

/**
 * Splits a string into an array of tokens based on whitespace
 * characters (spaces, tabs, and newlines).
 *
 * @param str    The string to split into tokens.
 * @param tokens An array to store the tokens.
 * @return       The number of tokens in the tokens array.
 */
int splitString(char *str, char *tokens[]);

void execute_command(char *message, char *response,
                     struct turtile_context *context) {
    char *tokens[MAX_MSG_ELEMENTS];
    int ntokens = splitString(message, tokens);

    for (int i = 0; commands[i].cmd_name != NULL; i++) {
        if (strcmp(tokens[0], commands[i].cmd_name) == 0) {
            if (commands[i].subcmd_name == NULL || 
                (ntokens > 1 && strcmp(tokens[1], commands[i].subcmd_name) == 0)) {
				// Call function with appropriate parameters
                commands[i].cmd_fun(tokens + (commands[i].subcmd_name ? 2 : 1), 
                                    ntokens - (commands[i].subcmd_name ? 2 : 1), 
                                    response, context);
                return;
            }
        }
    }
    snprintf(response, MAX_MSG_SIZE, "{\"error\": \"Unknown command %s\"}", message);
}
		
int splitString(char *str, char *tokens[]){
	int i=1;

    if ((tokens[0]=strtok(str," \n\t"))==NULL)
        return 0;
    while ((tokens[i]=strtok(NULL," \n\t"))!=NULL)
        i++;

    return i;
}

void exit_command(char *tokens[], int ntokens, char *response,
					struct turtile_context *context){
	struct turtile_server *server = context->server;
    wl_display_terminate(server->wl_display);
    snprintf(response, MAX_MSG_SIZE, "Exiting turtile\n");
}

void window_command(char *tokens[], int ntokens, char *response,
					struct turtile_context *context){
	// TODO: use this function as a help for the other window subcommands
    snprintf(response, MAX_MSG_SIZE,
			 "TODO: placeholder for window command help\n");
}

void window_list_command(char *tokens[], int ntokens, char *response,
						 struct turtile_context *context) {
    struct turtile_server *server = context->server;
    if (!server || wl_list_empty(&server->toplevels)) {
        response = strdup("{\"error\": \"No windows found\"}\n");
        return;
    }

    struct json_object *json_response = json_object_new_array();

    struct turtile_toplevel *toplevel;
    
    wl_list_for_each(toplevel, &server->toplevels, link) {
        if (toplevel->xdg_toplevel) {
            const char *title = toplevel->xdg_toplevel->title ?
				toplevel->xdg_toplevel->title : "Unnamed";
            const char *app = toplevel->xdg_toplevel->app_id ?
				toplevel->xdg_toplevel->title : "null";

            // Create a JSON object for each window and populate its fields
            struct json_object *json_window = json_object_new_object();
            json_object_object_add(json_window, "id",
								   json_object_new_string(toplevel->id));
            json_object_object_add(json_window, "app",
								   json_object_new_string(app));
            json_object_object_add(json_window, "title",
								   json_object_new_string(title));
            json_object_object_add(json_window, "workspace",
								   json_object_new_string(toplevel->workspace->name));
            
            json_object_array_add(json_response, json_window);
        }
    }

    strcpy(response, json_object_to_json_string(json_response));
    json_object_put(json_response);
}

void window_switch_command(char *tokens[], int ntokens, char *response,
					struct turtile_context *context){
	// Switch focus to designated toplevel
	struct turtile_server *server = context->server;

	if(ntokens >= 1){
		char *new_toplevel_id = tokens[0];
		struct turtile_toplevel *toplevel;

		wl_list_for_each(toplevel, &server->focus_toplevels, flink) {
			if(strcmp(toplevel->id, new_toplevel_id) == 0){
				focus_toplevel(toplevel, toplevel->xdg_toplevel->base->surface);
				snprintf(response, MAX_MSG_SIZE,
						 "{\"success\": \"switching focus to: %s\"}",
						 toplevel->xdg_toplevel->title);
				return;
			}
		}
		snprintf(response, MAX_MSG_SIZE,
				 "{\"error\": \"window %s not found\"}", new_toplevel_id);

	} else{
		snprintf(response, MAX_MSG_SIZE,
				 "{\"error\": \"missing argument: window id\"}");
	}
}

void window_cycle_command(char *tokens[], int ntokens, char *response,
					struct turtile_context *context){
	// Cycle to the next toplevel in the same workspace
	struct turtile_server *server = context->server;

	struct wl_list workspace_toplevels; 
	get_workspace_toplevels(server->active_workspace, &workspace_toplevels);

	if (wl_list_empty(&workspace_toplevels)){
		snprintf(response, MAX_MSG_SIZE,
				 "{\"error\": \"Workspace is empty\"}");
		return;
	} else if (wl_list_length(&workspace_toplevels) < 2) {
		snprintf(response, MAX_MSG_SIZE,
				 "{\"error\": \"Only one current window open\"}");
		return;
	} 		

	struct turtile_toplevel *next_toplevel =
		wl_container_of(workspace_toplevels.next, next_toplevel, auxlink);
	focus_toplevel(next_toplevel, next_toplevel->xdg_toplevel->base->surface);
    snprintf(response, MAX_MSG_SIZE, "{\"success\": \"switching focus to: %s\"}",
			 next_toplevel->xdg_toplevel->title);
}

void window_kill_command(char *tokens[], int ntokens, char *response,
					struct turtile_context *context){
	// kill designated toplevel
	struct turtile_server *server = context->server;
	struct turtile_toplevel *toplevel;

	if(ntokens >= 1){
		char *new_toplevel_id = tokens[0];

		wl_list_for_each(toplevel, &server->focus_toplevels, flink) {
			if(strcmp(toplevel->id, new_toplevel_id) == 0){
				kill_toplevel(toplevel);
				snprintf(response, MAX_MSG_SIZE,
						 "{\"success\": \"kill: %s\"}",
						 toplevel->xdg_toplevel->title);
				return;
			}
		}
		snprintf(response, MAX_MSG_SIZE,
				 "{\"error\": \"window %s not found\"}", new_toplevel_id);

	} else{
		toplevel = get_first_focus_toplevel(server);
		kill_toplevel(toplevel);
		snprintf(response, MAX_MSG_SIZE,
				 "{\"success\": \"kill: %s\"}", toplevel->xdg_toplevel->title);
	}
}

void window_move_to_command(char *tokens[], int ntokens, char *response,
							struct turtile_context *context){
	struct turtile_server *server = context->server;

    if (ntokens >= 1) {
		char *target_workspace_name = tokens[0];
		struct turtile_workspace *target_workspace =
			get_workspace(server, target_workspace_name);

		if (!target_workspace) {
			snprintf(response, MAX_MSG_SIZE,
					 "{\"error\": \"workspace not found\"}");
			return;
		}

		struct turtile_toplevel *toplevel_to_move;
		if (ntokens >= 2) {
			char *toplevel_id = tokens[1];
			
			toplevel_to_move = get_toplevel(server, toplevel_id);
			
			if (!toplevel_to_move) {
				snprintf(response, MAX_MSG_SIZE,
						 "{\"error\": \"window %s not found\"}", toplevel_id);
				return;
			}
		} else {
			toplevel_to_move = get_first_focus_toplevel(server);
			if (!toplevel_to_move) {
				snprintf(response, MAX_MSG_SIZE,
						 "{\"error\": \"no focused window to move\"}");
				return;
			}
		}
		
		toplevel_to_move->workspace = target_workspace;
		server_redraw_windows(server);
		
		snprintf(response, MAX_MSG_SIZE,
				 "{\"success\": \"moved window %s to workspace %s\"}",
				 toplevel_to_move->xdg_toplevel->title, target_workspace->name);

    } else {
        snprintf(response, MAX_MSG_SIZE,
                 "{\"error\": \"missing argument: workspace name\"}");
	}
}

void window_master_toggle_command(char *tokens[], int ntokens, char *response,
								  struct turtile_context *context){
	// Set designated toplevel as master
	struct turtile_server *server = context->server;
	struct turtile_toplevel *toplevel;

	if(ntokens >= 1){
		char *toplevel_id = tokens[0];

		toplevel = get_toplevel(server, toplevel_id);

		if(toplevel != NULL){
			set_master_toplevel(toplevel);

			snprintf(response, MAX_MSG_SIZE,
					 "{\"success\": \"master: %s\"}",
					 toplevel->xdg_toplevel->title);
			return;
		} else {
			snprintf(response, MAX_MSG_SIZE,
					 "{\"error\": \"window %s not found\"}", toplevel_id);
			return;
		}
	} else{
		toplevel = get_first_focus_toplevel(server);
		if(toplevel == get_first_toplevel(server)){

			struct turtile_toplevel *next_toplevel =
				get_next_focus_toplevel(server);
			if(next_toplevel != NULL){
				set_master_toplevel(get_next_focus_toplevel(server));
				snprintf(response, MAX_MSG_SIZE,
						 "{\"success\": \"master: %s\"}",
						 toplevel->xdg_toplevel->title);
				return;
			} else {
				snprintf(response, MAX_MSG_SIZE,
						 "{\"error\": \"the current window is already master\"}");
				return;
			}
		} else if(toplevel != NULL) {
			set_master_toplevel(toplevel);
			snprintf(response, MAX_MSG_SIZE,
					 "{\"success\": \"master: %s\"}",
					 toplevel->xdg_toplevel->title);
			return;
		}
	}
	snprintf(response, MAX_MSG_SIZE, "{\"error\": \"no window found\"}");
}

void workspace_command(char *tokens[], int ntokens, char *response,
					   struct turtile_context *context){
	// TODO: use this function as a help for the other workspace subcommands
    snprintf(response, MAX_MSG_SIZE,
			 "TODO: placeholder for workspace command help\n");
}

void workspace_list_command(char *tokens[], int ntokens, char *response,
                            struct turtile_context *context) {
    struct turtile_server *server = context->server;
    if (!server || wl_list_empty(&server->workspaces)) {
        response = strdup("{\"error\": \"No workspaces found\"}\n");
        return;
    }

    struct json_object *json_response = json_object_new_array();

    struct turtile_workspace *workspace;
    wl_list_for_each(workspace, &server->workspaces, link) {
        const char *name = workspace->name;
        bool is_active = (workspace == server->active_workspace);

        // Create a JSON object for each workspace and populate its fields
        struct json_object *json_workspace = json_object_new_object();
        json_object_object_add(json_workspace, "name",
                               json_object_new_string(name));
        json_object_object_add(json_workspace, "active",
                               json_object_new_boolean(is_active));

        json_object_array_add(json_response, json_workspace);
    }

    strcpy(response, json_object_to_json_string(json_response));
    json_object_put(json_response);
}

void workspace_switch_command(char *tokens[], int ntokens, char *response,
							  struct turtile_context *context){
	struct turtile_server *server = context->server;

	if(ntokens >= 1){
		char *new_workspace_name = tokens[0];
		struct turtile_workspace *workspace;

		if(strcmp(server->active_workspace->name, new_workspace_name) == 0){
            snprintf(response, MAX_MSG_SIZE,
                     "{\"success\": \"already in workspace %s\"}\n",
					 new_workspace_name);
            return;
        }
		wl_list_for_each(workspace, &server->workspaces, link) {
			if(strcmp(workspace->name, new_workspace_name) == 0){
				switch_workspace(workspace);
				snprintf(response, MAX_MSG_SIZE,
						 "{\"success\": \"switch to workspace %s\"}",
						 new_workspace_name);
				return;
			}
		}
		snprintf(response, MAX_MSG_SIZE,
				 "{\"error\": \"workspace %s not found\"}", new_workspace_name);

	} else{
		snprintf(response, MAX_MSG_SIZE,
				 "{\"error\": \"missing argument: workspace name\"}");
	}
}
