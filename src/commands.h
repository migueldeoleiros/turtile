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

#ifndef COMMANDS_H
#define COMMANDS_H
#include "src/server.h"

struct turtile_context{
	struct turtile_server *server;
};

/**
 * Execute a command with the given message and context.
 *
 * This function takes a message string, splits it into tokens, and executes the
 * corresponding command based on the tokens. If the command is not recognized,
 * it returns an error message.
 *
 * @param message  The message string to execute as a command.
 * @param response A buffer to store the response to the command.
 * @param context  A pointer to the turtile context structure.
 */
void execute_command(char *message, char *response,
					 struct turtile_context *context);

#endif // COMMANDS_H
