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
#include "src/toplevel.h"
#include <stdio.h>
#include <string.h>
#include <wlr/types/wlr_xdg_shell.h>

// Declare functions so that they can be referenced in the list |commands|
void window_command(char *tokens[], int ntokens, char *response,
					struct turtile_context *context);
void window_list_command(char *tokens[], int ntokens, char *response,
						 struct turtile_context *context);

typedef struct {
    char *cmd_name;
    char *subcmd_name;
    void (*cmd_fun)(char *tokens[], int ntokens, char *response,
                   struct turtile_context *context);
} command_t;

// List of commands with their associated functions 
static command_t commands[] = {
    {"window", "list", window_list_command},
    {"window", NULL, window_command},
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
    snprintf(response, MAX_MSG_SIZE, "FUnknown command: %s\n", message);
}
		
int splitString(char *str, char *tokens[]){
	int i=1;

    if ((tokens[0]=strtok(str," \n\t"))==NULL)
        return 0;
    while ((tokens[i]=strtok(NULL," \n\t"))!=NULL)
        i++;

    return i;
}

void window_command(char *tokens[], int ntokens, char *response,
					struct turtile_context *context){
	// TODO: use this function as a help for the other window subcommands
    snprintf(response, MAX_MSG_SIZE,
			 "TODO: placeholder for window command help\n");
}

void window_list_command(char *tokens[], int ntokens, char *response,
						 struct turtile_context *context){
	struct turtile_server *server = context->server;
    if (!server || wl_list_empty(&server->toplevels)) {
		snprintf(response, MAX_MSG_SIZE, "No windows found.\n");
        return;
    }

    struct turtile_toplevel *toplevel;
    size_t offset = 0; // Track the current offset in the buffer

    wl_list_for_each(toplevel, &server->toplevels, link) {
        if (toplevel->xdg_toplevel) {
            const char *title = toplevel->xdg_toplevel->title ?
                toplevel->xdg_toplevel->title : "Unnamed";

            // Calculate remaining buffer space and append the title
            int written = snprintf(response + offset, MAX_MSG_SIZE - offset,
								   "Window: %s\n", title);
            if (written < 0 || (size_t)written >= MAX_MSG_SIZE - offset) {
                // Stop appending if there's not enough space left in the buffer
                break;
            }
            offset += written;
        }
    }
}
