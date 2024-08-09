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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "socket_server.h"

void handle_client(int client_socket);
void execute_command(const char *command, char *response);

void start_socket_server() {
    int server_socket, client_socket;
    struct sockaddr_un server_address;

    // Remove socket if it already exists
    unlink(SOCKET_PATH);

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, SOCKET_PATH,
			sizeof(server_address.sun_path) - 1);

    if (bind(server_socket, (struct sockaddr *)&server_address,
			 sizeof(server_address)) == -1) {
        perror("Failed to bind socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Failed to listen on socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on %s\n", SOCKET_PATH);

    while (1) {
        client_socket = accept(server_socket, NULL, NULL);
        if (client_socket == -1) {
            perror("Failed to accept client connection");
            continue;
        }

        handle_client(client_socket);
    }

    close(server_socket);
    unlink(SOCKET_PATH);
}

void handle_client(int client_socket) {
    char buffer[MAX_MSG_SIZE];
    char response[MAX_MSG_SIZE];

    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0'; // Null-terminate the command string

        printf("Received command: %s\n", buffer);

        execute_command(buffer, response);

        send(client_socket, response, strlen(response), 0);
    }

    close(client_socket);
}

void execute_command(const char *command, char *response) {
    // TODO: Implement actual command handling and execution logic

    if (strcmp(command, "hello world") == 0) {
        snprintf(response, MAX_MSG_SIZE, "Hello, World!\n");
    } else {
        snprintf(response, MAX_MSG_SIZE, "Unknown command: %s\n", command);
    }
}
