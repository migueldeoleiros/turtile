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

#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "server.h"

struct turtile_workspace {
    struct wl_list link;

	char name[100];
	struct turtile_server *server;
	// TODO: add associated output for indendent workspaces in each display
};

/**
 * Creates a new workspace and adds it to the server's workspace list.
 *
 * @param server The server instance to which the workspace will be added.
 * @param name The name of the new workspace.
 *
 * @return A pointer to the newly created workspace, or NULL if the creation fails.
 */
struct turtile_workspace* create_workspace(struct turtile_server *server,
										  char *name);
/**
 * Switches the active workspace to the specified workspace.
 *
 * @param workspace The workspace to switch to
 */
void switch_workspace(struct turtile_workspace *workspace);

/**
 * Creates workspaces from the configuration and adds them to the server's
 * workspace list.
 *
 * @param server The server instance to which the workspaces will be added.
 *
 * @return A pointer to the last created workspace, or
 *         NULL if no workspaces were created.
 */
struct turtile_workspace* create_workspaces_from_config(struct turtile_server *server);

#endif // WORKSPACE_H
