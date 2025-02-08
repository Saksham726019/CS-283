#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    // Initialize clist with 0 using memset().
    memset(clist, 0, sizeof(command_list_t));

    // Split the cmd_line.
    char* token = strtok(cmd_line, PIPE_STRING);

    while (token != NULL) 
    {
        // Check if the total number of commands exceed CMD_MAX.
        if (clist->num >= CMD_MAX)
        {
            return ERR_TOO_MANY_COMMANDS;
        }
        
        // Remove leading whitespaces.
        while (*token == SPACE_CHAR)
        {
            token++;
        }

        // Get the args. Part I
        char *args = strchr(token, SPACE_CHAR);

        if (args != NULL)
        {
            // If we have args, then we need to put null terminator to mark it as end of exe.
            *args = '\0';

            // Start of args.
            args ++;
            
            // Remove leading spaces in arguments.
            while (*args == SPACE_CHAR) 
            {
                args++;
            }

            // Remove trailing spaces in arguments.
            int k = 1;
            int args_len = strlen(args);

            while (args_len - k >= 0 && args[args_len - k] == SPACE_CHAR)
            {
                args[args_len - k] = '\0';
                k++;
            }

            // Check if arguments length is too big.
            if (strlen(args) >= ARG_MAX) 
            {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }

            // Copy args to args.
            strcpy(clist->commands[clist->num].args, args);

        } else if (args == NULL)
        {
            clist->commands[clist->num].args[0] = '\0';
        }

        // Check if any command's length is too big.
        if (strlen(token) >= EXE_MAX)
        {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        // Copy token to exe.
        strcpy(clist->commands[clist->num].exe, token);

        clist->num++;

        // Move to the next command.
        token = strtok(NULL, PIPE_STRING);
    }

    return OK;

}