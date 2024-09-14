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

   This code was also inspired from BSPWM with a BSD 2-Clause License
   Copyright (c) 2012, Bastien Dejean 
   <https://github.com/baskerville/bspwm>
   ----------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define MAX_MSG_SIZE 1024
#define SOCKET_PATH "/tmp/turtile_socket"

int main(int argc, char *argv[]) {
    int socket_fd;
    struct sockaddr_un socket_address;
    char message[MAX_MSG_SIZE];

    if (argc < 2) {
		// TODO: replace with help function
        fprintf(stderr, "No arguments given");
        return EXIT_FAILURE;
    }
    // Set up socket address
    socket_address.sun_family = AF_UNIX;
    strcpy(socket_address.sun_path, SOCKET_PATH);

    // Create socket
    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("Failed to create socket");
        return EXIT_FAILURE;
    }

    // Connect to socket
    if (connect(socket_fd, (struct sockaddr *)&socket_address,
				sizeof(socket_address)) == -1) {
        perror("Failed to connect to socket");
        return EXIT_FAILURE;
    }

    // Build message from arguments
    int message_len = 0;
    for (int i = 1; i < argc; i++) {
        if (i > 1){ // Add spaces
            message_len += snprintf(message + message_len,
									MAX_MSG_SIZE - message_len, " ");
		}
		message_len += snprintf(message + message_len,
								MAX_MSG_SIZE - message_len, "%s", argv[i]);
    }

    // Send message
    if (send(socket_fd, message, message_len, 0) == -1) {
        perror("Failed to send message");
        return EXIT_FAILURE;
    }

    // Receive response size
    ssize_t response_size;
    if (recv(socket_fd, &response_size, sizeof(response_size), 0) <= 0) {
        perror("Failed to receive response size");
        return EXIT_FAILURE;
    }

    // Allocate buffer for full response
    char *response = malloc(response_size + 1);
    if (!response) {
        perror("Failed to allocate memory for response");
        return EXIT_FAILURE;
    }

    // Receive full response
    ssize_t total_received = 0;
    while (total_received < response_size) {
        ssize_t bytes_received = recv(socket_fd, response + total_received,
                                      response_size - total_received, 0);
        if (bytes_received <= 0) {
            break;
        }
        total_received += bytes_received;
    }
    response[total_received] = '\0'; // Null-terminate the response

    printf("%s", response);

    free(response);
    close(socket_fd);
    return EXIT_SUCCESS;
}
