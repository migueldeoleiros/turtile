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

#include "src/server.h"
#include <stdbool.h>

#define MAX_MSG_SIZE 1024
#define SOCKET_PATH "/tmp/turtile_socket"

//TODO: add documentation comment
void* start_socket_server(void *server_ptr);

#endif // SOCKET_SERVER_H
