#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"


int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{
    // Initialize cmd_buff with 0 using memset().
    memset(cmd_buff, 0, sizeof(cmd_buff_t));

    // Store the copy of the cmd buffer into _cmd_buff.
    cmd_buff->_cmd_buffer = strdup(cmd_line);
    if (!cmd_buff->_cmd_buffer) 
    {
        return ERR_MEMORY;
    }

    bool inside_quotes = false;
    int arg_count = 0;

    while (*cmd_line) 
    {
        // Skip leading spaces until we hit a quote.
        while (*cmd_line == SPACE_CHAR && !inside_quotes)
        {
            cmd_line++;
        }

        // If we reach null terminator, and if arg_count is 0, then only spaces were given in the command.
        if (*cmd_line == '\0')
        {
            if (arg_count > 0)
            {
                break;
            } else
            {
                free(cmd_buff->_cmd_buffer);
                return WARN_NO_CMDS;    
            }
            
        }

        if (*cmd_line == QUOTES)
        {
            inside_quotes = !inside_quotes;
            cmd_line++;
        }

        // Store the pointer to the first char of the argument.
        cmd_buff->argv[arg_count] = cmd_line;
        arg_count++;

        // Terminate the program if arg_count or arg length is max.
        if (arg_count > 0 && (strlen(cmd_buff->argv[arg_count - 1]) >= ARG_MAX || arg_count >= CMD_ARGV_MAX))
        {
            free(cmd_buff->_cmd_buffer);
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }  

        // If not inside quotes, we stop iterating once we hit a space.
        // If inside quotes, we won't stop on space, but stop once we hit the second quotes.
        while (*cmd_line && (*cmd_line != SPACE_CHAR || inside_quotes))
        {
            if (*cmd_line == QUOTES)
            {
                inside_quotes = !inside_quotes;
                *cmd_line = '\0';
            }
            cmd_line++;
        }
        
        // Null terminate the argument.
        if (*cmd_line == SPACE_CHAR)
        {
            *cmd_line = '\0';
            cmd_line++;
        }      
    }

    cmd_buff->argc = arg_count;
    cmd_buff->argv[arg_count] = NULL;

    return OK;
}


int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    // Initialize clist with 0 using memset().
    memset(clist, 0, sizeof(command_list_t));
    int rc = 0;

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

        // Remove trailing spaces.
        int token_len = strlen(token);
        while (token_len > 0 && token[token_len - 1] == SPACE_CHAR)
        {
            token[token_len - 1] = '\0';
            token_len--;
        }

        // Check if any command's length is too big.
        if (strlen(token) >= EXE_MAX)
        {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        rc = build_cmd_buff(token, &clist->commands[clist->num]);

        if (rc != OK)
        {
            return rc;
        }

        clist->num++;

        // Move to the next token.
        token = strtok(NULL, PIPE_STRING);
    }

    return OK;
}


/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop()
{
    char *cmd_buff = malloc(SH_CMD_MAX);
    int rc = 0;
    cmd_buff_t cmd;
    command_list_t clist;

    while (1)
    {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL)
        {
            printf("\n");
            break;
        }
        // remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        // IMPLEMENT THE REST OF THE REQUIREMENTS

        // If not user command given, continue the while loop.
        if (strlen(cmd_buff) == 0)
        {
            printf("%s", CMD_WARN_NO_CMD);
            continue;

        } else if (strcmp(cmd_buff, EXIT_CMD) == 0)     // If exit is given as the command, then exit the program.
        {
            free(cmd_buff);
            exit(OK);
        }

        if (strncmp(cmd_buff, "cd", 2) == 0 && (cmd_buff[2] == '\0' || cmd_buff[2] == ' '))
        {
            strtok(cmd_buff, " ");
            char* arg = strtok(NULL, " ");

            // do nothing if no args passed for cd.
            if (arg == NULL)
            {
                continue;
            } else
            {
                // cd can only handle one argument. So, check if there is more than one argument.
                char* extraArgs = strtok(NULL, " ");

                if (extraArgs != NULL)
                {
                    printf("cd: error too many arguments!");
                    return ERR_TOO_MANY_COMMANDS;
                }
                
                if (chdir(arg) != 0)
                {
                    printf(CMD_ERR_EXECUTE);
                    continue;
                }
            }
            continue;
        }

        rc = build_cmd_list(cmd_buff, &clist);

        if (rc == OK)
        {
            printf(CMD_OK_HEADER, clist.num);

            for (int i = 0; i < clist.num; i++)
            {
                printf("<%d> ", i + 1);
                for (int j = 0; j < clist.commands[i].argc; j++)
                {
                    printf("%s ", clist.commands[i].argv[j]);
                }
                printf("\n");
                // Free the dynamically allocated _cmd_buffer for each command.
                free(clist.commands[i]._cmd_buffer);
            }

        } else
        {
            if (rc == ERR_TOO_MANY_COMMANDS)
            {
                printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            } else if (rc == ERR_CMD_OR_ARGS_TOO_BIG)
            {
                printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            } else if (rc == WARN_NO_CMDS)
            {
                printf(CMD_WARN_NO_CMD);
            }
            
            continue;
        }
    }

    free(cmd_buff);
    return OK;
}
