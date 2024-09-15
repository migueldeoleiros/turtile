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

#include "json_tokener.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <json-c/json.h>

#define MAX_MSG_SIZE 1024
#define SOCKET_PATH "/tmp/turtile_socket"

void print_json_object(json_object *obj, int indent) {
    enum json_type type;

    type = json_object_get_type(obj);
    switch (type) {
        case json_type_string:
            printf("%s\n", json_object_get_string(obj));
            break;
        case json_type_int:
            printf("%d\n", json_object_get_int(obj));
            break;
        case json_type_double:
            printf("%f\n", json_object_get_double(obj));
            break;
        case json_type_boolean:
            printf("%s\n", json_object_get_boolean(obj) ? "true" : "false");
            break;
	    case json_type_object: {
            json_object_object_foreach(obj, key, obj2) {
                printf("%*s%s: ", indent * 2, "", key);
                print_json_object(obj2, indent + 1);
            }
            printf("%*s\n", indent * 2, "");
            break;
		
	    }
     	case json_type_array: {
            int arraylen = json_object_array_length(obj);
            for (int i = 0; i < arraylen; i++) {
                printf("%*s", indent * 2, "");
                print_json_object(json_object_array_get_idx(obj, i), indent + 1);
            }
            printf("%*s", indent * 2, "");
            break;
        }
        default:
            printf("Unknown type\n");
            break;
    }
}

void process_command_output(const char *str, bool human_readable) {
    struct json_object *parsed_json = json_tokener_parse(str);

    if (human_readable && parsed_json != NULL) {
        print_json_object(parsed_json, 0);
    } else {
        printf("%s", str);
    }

    json_object_put(parsed_json);
}

int main(int argc, char *argv[]) {
    int socket_fd;
    struct sockaddr_un socket_address;
    char message[MAX_MSG_SIZE];
    bool human_readable = true; // Flag to track if --json is passed

    if (argc < 2) {
        // TODO: replace with help function
        fprintf(stderr, "No arguments given\n");
        return EXIT_FAILURE;
    }

    // Check if the first argument is "--json"
    int arg_start = 1;
    if (strcmp(argv[1], "--json") == 0) {
        human_readable = false;
        arg_start = 2; // Skip the --json argument
        if (argc < 3) {
            fprintf(stderr, "No command given after --json\n");
            return EXIT_FAILURE;
        }
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

    // Build message from arguments, skipping --json if it was present
    int message_len = 0;
    for (int i = arg_start; i < argc; i++) {
        if (i > arg_start) { // Add spaces between arguments
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

    process_command_output(response, human_readable);

    free(response);
    close(socket_fd);
    return EXIT_SUCCESS;
}
