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
#include <stdio.h>
#include <string.h>

typedef struct {
    char *cmd_name;
    char *subcmd_name;
    void (*cmd_fun)(char *tokens[], int ntokens, char *response,
                   struct turtile_context *context);
} command_t;


//TODO: add documentation comments
void execute_subcommand(char *tokens[], int ntokens, char *response,
                        struct turtile_context *context,
						command_t *subcommands);
int splitString(char *str, char *tokens[]);


//TODO: add documentation comments
void window_command(char *tokens[], int ntokens, char *response,
					struct turtile_context *context);
void window_list_command(char *tokens[], int ntokens, char *response,
						 struct turtile_context *context);


static command_t commands[] = {
    {"window", "list", window_list_command},
    {"window", NULL, window_command},
    {NULL, NULL, NULL} // Terminate array with NULLs
};

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
    snprintf(response, MAX_MSG_SIZE, "Unknown command: %s\n", message);
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
    snprintf(response, MAX_MSG_SIZE,
			 "TODO: placeholder for window list\n");
}
