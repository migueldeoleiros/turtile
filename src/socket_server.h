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

#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include "server.h"
#include "commands.h"
#include <stdbool.h>

#define MAX_MSG_SIZE 1024 // max size of both commands and responses
#define MAX_MSG_ELEMENTS 5 // max number of params in a command
#define SOCKET_PATH "/tmp/turtile_socket"

/**
 * Start a socket server that listens for incoming client connections.
 *
 * This function creates a Unix domain socket, binds it to a specific path,
 * and listens for incoming connections.
 *
 * @param context_ptr A context pointer of type turtile_context that is going
 *                    to be sent to the commands.
 * @return A void pointer for multithreading.
 */
void* start_socket_server(void *context_ptr);

#endif // SOCKET_SERVER_H
