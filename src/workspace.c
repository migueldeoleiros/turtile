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

#include "workspace.h"
#include "src/config.h"
#include "src/server.h"
#include "src/toplevel.h"
#include "wlr/util/log.h"
#include <stdlib.h>
#include <string.h>
#include <wayland-util.h> 

struct turtile_workspace* create_workspace(struct turtile_server *server,
										  char *name){
	struct turtile_workspace *new_workspace =
		malloc(sizeof(struct turtile_workspace));
	strcpy(new_workspace->name, name);
	new_workspace->server = server;
	
    wl_list_insert(&server->workspaces, &new_workspace->link);
    wlr_log(WLR_INFO, "Create workspace: %s", new_workspace->name);
	return new_workspace;
}

struct turtile_workspace *get_workspace(struct turtile_server *server,
										char *name) {
	struct turtile_workspace *workspace;
	wl_list_for_each(workspace, &server->workspaces, link) {
		if (strcmp(workspace->name, name) == 0) {
			return workspace;
		}
	}
	return NULL;
}

void switch_workspace(struct turtile_workspace *workspace){
	if(workspace == NULL){
		return;
	}
	struct turtile_server *server = workspace->server;
	server->active_workspace = workspace;

	struct turtile_toplevel *newfocus = get_first_focus_toplevel(server);
	if(newfocus != NULL)
		focus_toplevel(newfocus, newfocus->xdg_toplevel->base->surface);

	server_redraw_windows(server);
}

struct turtile_workspace* create_workspaces_from_config(struct turtile_server *server) {
    turtile_workspace_config_t *workspace_config;

	struct turtile_workspace *active_workspace;

    wl_list_for_each(workspace_config, &config_get_instance()->workspaces, link) {
		active_workspace = create_workspace(server, workspace_config->name);
	}
	return active_workspace; 
}

void get_workspace_toplevels(struct turtile_workspace *workspace,
										struct wl_list *toplevels) {
    struct turtile_server *server = workspace->server;
    wl_list_init(toplevels);

    struct turtile_toplevel *toplevel;
    wl_list_for_each(toplevel, &server->focus_toplevels, flink)
        if (toplevel->workspace == workspace)
            wl_list_insert(toplevels, &toplevel->auxlink);
}
