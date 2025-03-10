
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <fcntl.h>

//INCLUDES for extra credit
//#include <signal.h>
//#include <pthread.h>
//-------------------------

#include "dshlib.h"
#include "rshlib.h"


/*
 * boot_server(ifaces, port)
 *      ifaces & port:  see start_server for description.  They are passed
 *                      as is to this function.   
 * 
 *      This function "boots" the rsh server.  It is responsible for all
 *      socket operations prior to accepting client connections.  Specifically: 
 * 
 *      1. Create the server socket using the socket() function. 
 *      2. Calling bind to "bind" the server to the interface and port
 *      3. Calling listen to get the server ready to listen for connections.
 * 
 *      after creating the socket and prior to calling bind you might want to 
 *      include the following code:
 * 
 *      int enable=1;
 *      setsockopt(svr_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
 * 
 *      when doing development you often run into issues where you hold onto
 *      the port and then need to wait for linux to detect this issue and free
 *      the port up.  The code above tells linux to force allowing this process
 *      to use the specified port making your life a lot easier.
 * 
 *  Returns:
 * 
 *      server_socket:  Sockets are just file descriptors, if this function is
 *                      successful, it returns the server socket descriptor, 
 *                      which is just an integer.
 * 
 *      ERR_RDSH_COMMUNICATION:  This error code is returned if the socket(),
 *                               bind(), or listen() call fails. 
 * 
 */
int boot_server(char *ifaces, int port){
    int svr_socket;
    int ret;
    
    struct sockaddr_in addr;

    // TODO set up the socket - this is very similar to the demo code

    // Create socket.
    svr_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (svr_socket == -1)
    {
        perror("socket");
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }

    int enable=1;
    setsockopt(svr_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    /* Bind socket to socket name. */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ifaces);
    addr.sin_port = htons(port);
    
    ret = bind(svr_socket, (const struct sockaddr *) &addr, sizeof(struct sockaddr_in));

    if (ret == -1)
    {
        perror("accept");
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }

    /*
     * Prepare for accepting connections. The backlog size is set
     * to 20. So while one request is being processed other requests
     * can be waiting.
     */
    ret = listen(svr_socket, 20);
    if (ret == -1) 
    {
        perror("listen");
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }

    // For debugging. Remove later.
    printf("Server booted successfully on %s:%d\n", ifaces, port);

    return svr_socket;
}


/*
 * start_server(ifaces, port, is_threaded)
 *      ifaces:  a string in ip address format, indicating the interface
 *              where the server will bind.  In almost all cases it will
 *              be the default "0.0.0.0" which binds to all interfaces.
 *              note the constant RDSH_DEF_SVR_INTFACE in rshlib.h
 * 
 *      port:   The port the server will use.  Note the constant 
 *              RDSH_DEF_PORT which is 1234 in rshlib.h.  If you are using
 *              tux you may need to change this to your own default, or even
 *              better use the command line override -s implemented in dsh_cli.c
 *              For example ./dsh -s 0.0.0.0:5678 where 5678 is the new port  
 * 
 *      is_threded:  Used for extra credit to indicate the server should implement
 *                   per thread connections for clients  
 * 
 *      This function basically runs the server by: 
 *          1. Booting up the server
 *          2. Processing client requests until the client requests the
 *             server to stop by running the `stop-server` command
 *          3. Stopping the server. 
 * 
 *      This function is fully implemented for you and should not require
 *      any changes for basic functionality.  
 * 
 *      IF YOU IMPLEMENT THE MULTI-THREADED SERVER FOR EXTRA CREDIT YOU NEED
 *      TO DO SOMETHING WITH THE is_threaded ARGUMENT HOWEVER.  
 */
int start_server(char *ifaces, int port, int is_threaded){
    int svr_socket;
    int rc;

    //
    //TODO:  If you are implementing the extra credit, please add logic
    //       to keep track of is_threaded to handle this feature
    //

    svr_socket = boot_server(ifaces, port);
    if (svr_socket < 0){
        int err_code = svr_socket;  //server socket will carry error code
        return err_code;
    }

    rc = process_cli_requests(svr_socket);

    stop_server(svr_socket);


    return rc;
}

/*
 * stop_server(svr_socket)
 *      svr_socket: The socket that was created in the boot_server()
 *                  function. 
 * 
 *      This function simply returns the value of close() when closing
 *      the socket.  
 */
int stop_server(int svr_socket){
    return close(svr_socket);
}


/*
 * process_cli_requests(svr_socket)
 *      svr_socket:  The server socket that was obtained from boot_server()
 *   
 *  This function handles managing client connections.  It does this using
 *  the following logic
 * 
 *      1.  Starts a while(1) loop:
 *  
 *          a. Calls accept() to wait for a client connection. Recall that 
 *             the accept() function returns another socket specifically
 *             bound to a client connection. 
 *          b. Calls exec_client_requests() to handle executing commands
 *             sent by the client. It will use the socket returned from
 *             accept().
 *          c. Loops back to the top (step 2) to accept connecting another
 *             client.  
 * 
 *          note that the exec_client_requests() return code should be
 *          negative if the client requested the server to stop by sending
 *          the `stop-server` command.  If this is the case step 2b breaks
 *          out of the while(1) loop. 
 * 
 *      2.  After we exit the loop, we need to cleanup.  Dont forget to 
 *          free the buffer you allocated in step #1.  Then call stop_server()
 *          to close the server socket. 
 * 
 *  Returns:
 * 
 *      OK_EXIT:  When the client sends the `stop-server` command this function
 *                should return OK_EXIT. 
 * 
 *      ERR_RDSH_COMMUNICATION:  This error code terminates the loop and is
 *                returned from this function in the case of the accept() 
 *                function failing. 
 * 
 *      OTHERS:   See exec_client_requests() for return codes.  Note that positive
 *                values will keep the loop running to accept additional client
 *                connections, and negative values terminate the server. 
 * 
 */
int process_cli_requests(int svr_socket){
    int cli_socket;
    int rc = OK;
    struct sockaddr_in client_addr;

    while(1)
    {
        // TODO use the accept syscall to create cli_socket 
        // and then exec_client_requests(cli_socket)

        cli_socket = accept(svr_socket, (struct sockaddr *) &client_addr, sizeof(struct sockaddr_in));

        if (cli_socket == -1)
        {
            perror("accept");
            rc = ERR_RDSH_COMMUNICATION;
            break;
        }
        
        // For debugging. Remove later.
        printf("Accepted connection from client.\n");
        
        // Call exec_client_requests to process commands from the client.
        rc = exec_client_requests(cli_socket);

        if (rc == OK_EXIT)
        {
            printf("Stop server command. Stopping the server.\n");
            stop_server(cli_socket);
            break;
        }
    }

    stop_server(cli_socket);
    return rc;
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
 * exec_client_requests(cli_socket)
 *      cli_socket:  The server-side socket that is connected to the client
 *   
 *  This function handles accepting remote client commands. The function will
 *  loop and continue to accept and execute client commands.  There are 2 ways
 *  that this ongoing loop accepting client commands ends:
 * 
 *      1.  When the client executes the `exit` command, this function returns
 *          to process_cli_requests() so that we can accept another client
 *          connection. 
 *      2.  When the client executes the `stop-server` command this function
 *          returns to process_cli_requests() with a return code of OK_EXIT
 *          indicating that the server should stop. 
 * 
 *  Note that this function largely follows the implementation of the
 *  exec_local_cmd_loop() function that you implemented in the last 
 *  shell program deliverable. The main difference is that the command will
 *  arrive over the recv() socket call rather than reading a string from the
 *  keyboard. 
 * 
 *  This function also must send the EOF character after a command is
 *  successfully executed to let the client know that the output from the
 *  command it sent is finished.  Use the send_message_eof() to accomplish 
 *  this. 
 * 
 *  Of final note, this function must allocate a buffer for storage to 
 *  store the data received by the client. For example:
 *     io_buff = malloc(RDSH_COMM_BUFF_SZ);
 *  And since it is allocating storage, it must also properly clean it up
 *  prior to exiting.
 * 
 *  Returns:
 * 
 *      OK:       The client sent the `exit` command.  Get ready to connect
 *                another client. 
 *      OK_EXIT:  The client sent `stop-server` command to terminate the server
 * 
 *      ERR_RDSH_COMMUNICATION:  A catch all for any socket() related send
 *                or receive errors. 
 */
int exec_client_requests(int cli_socket) {
    int io_size;
    command_list_t cmd_list;
    int rc;
    int cmd_rc;
    int last_rc;
    char *io_buff;

    io_buff = malloc(RDSH_COMM_BUFF_SZ);
    if (io_buff == NULL){
        return ERR_RDSH_SERVER;
    }

    // Variable to store how many bytes been received so far.
    int recv_total_size = 0;
    memset(io_buff, 0, RDSH_COMM_BUFF_SZ);

    while(1) {
        // TODO use recv() syscall to get input
        io_size = recv(cli_socket, io_buff + recv_total_size, RDSH_COMM_BUFF_SZ - recv_total_size, 0);

        if (io_size == -1)
        {
            perror("read error");
            free(io_buff);
            return ERR_RDSH_COMMUNICATION;
        }

        if (io_size == 0)
        {
            break;
        }
        
        recv_total_size += io_size;

        // If we didn't hit the null termination, 
        // check if we hit the max buff size, 
        // else continue adding the command to the io_buff.
        if (io_buff[recv_total_size - 1] != RDSH_EOF_CHAR)
        {
            if (recv_total_size >= RDSH_COMM_BUFF_SZ)
            {
                printf("Buffer overflow\n");
                free(io_buff);
                return ERR_RDSH_COMMUNICATION;
            }
            continue;            
        }
        
        // io_buff now holds complete command.
        if (strcmp(io_buff, EXIT_CMD) == 0)
        {
            send_message_string(cli_socket, RCMD_MSG_CLIENT_EXITED);
            send_message_eof(cli_socket);
            free(io_buff);
            return OK;
        }

        if (strcmp(io_buff, STOP_SERVER_CMD) == 0)
        {
            send_message_string(cli_socket, RCMD_MSG_SVR_STOP_REQ);
            send_message_eof(cli_socket);
            free(io_buff);
            return OK;
        }

        if (strncmp(io_buff, "cd", 2) == 0 && (io_buff[2] == '\0' || io_buff[2] == ' '))
        {
            strtok(io_buff, " ");
            char* arg = strtok(NULL, " ");

            // do nothing if no args passed for cd.
            if (arg == NULL)
            {
                recv_total_size = 0;
                memset(io_buff, 0, RDSH_COMM_BUFF_SZ);
                continue;
            } else
            {
                // cd can only handle one argument. So, check if there is more than one argument.
                char* extraArgs = strtok(NULL, " ");

                if (extraArgs != NULL)
                {
                    printf("cd: error too many arguments!");
                    free(io_buff);
                    return ERR_TOO_MANY_COMMANDS;
                }
                
                if (chdir(arg) != 0)
                {
                    printf(CMD_ERR_EXECUTE);
                    recv_total_size = 0;
                    memset(io_buff, 0, RDSH_COMM_BUFF_SZ);
                    continue;
                }
            }
            recv_total_size = 0;
            memset(io_buff, 0, RDSH_COMM_BUFF_SZ);
            continue;
        }

        // TODO build up a cmd_list
        rc = build_cmd_list(io_buff, &cmd_list);

        if (rc != OK)
        {
            send_message_string(cli_socket, "Error parsing command");
            send_message_eof(cli_socket);
        } else
        {
            // TODO rsh_execute_pipeline to run your cmd_list
            last_rc = rsh_execute_pipeline(cli_socket, &cmd_list);

            // TODO send appropriate respones with send_message_string
            char response[128];
            snprintf(response, sizeof(response), "Command executed with return code: %d", last_rc);
            
            // Send the response back to the client.
            send_message_string(cli_socket, response);

            // TODO send_message_eof when done
            send_message_eof(cli_socket);
            
            // Free resources allocated for the command list.
            free_cmd_list(&cmd_list);
        }

        // Reset for next command.
        recv_total_size = 0;
        memset(io_buff, 0, RDSH_COMM_BUFF_SZ);
    }
    
    free(io_buff);
    return OK;
}

/*
 * send_message_eof(cli_socket)
 *      cli_socket:  The server-side socket that is connected to the client

 *  Sends the EOF character to the client to indicate that the server is
 *  finished executing the command that it sent. 
 * 
 *  Returns:
 * 
 *      OK:  The EOF character was sent successfully. 
 * 
 *      ERR_RDSH_COMMUNICATION:  The send() socket call returned an error or if
 *           we were unable to send the EOF character. 
 */
int send_message_eof(int cli_socket){
    int send_len = (int)sizeof(RDSH_EOF_CHAR);
    int sent_len;
    sent_len = send(cli_socket, &RDSH_EOF_CHAR, send_len, 0);

    if (sent_len != send_len){
        return ERR_RDSH_COMMUNICATION;
    }
    return OK;
}


/*
 * send_message_string(cli_socket, char *buff)
 *      cli_socket:  The server-side socket that is connected to the client
 *      buff:        A C string (aka null terminated) of a message we want
 *                   to send to the client. 
 *   
 *  Sends a message to the client.  Note this command executes both a send()
 *  to send the message and a send_message_eof() to send the EOF character to
 *  the client to indicate command execution terminated. 
 * 
 *  Returns:
 * 
 *      OK:  The message in buff followed by the EOF character was 
 *           sent successfully. 
 * 
 *      ERR_RDSH_COMMUNICATION:  The send() socket call returned an error or if
 *           we were unable to send the message followed by the EOF character. 
 */
int send_message_string(int cli_socket, char *buff){
    //TODO implement writing to cli_socket with send()
    if (send(cli_socket, buff, strlen(buff) + 1, 0) < 0)
    {
        return ERR_RDSH_COMMUNICATION;
    }
    send_message_eof(cli_socket);

    return OK;
}


/*
 * rsh_execute_pipeline(int cli_sock, command_list_t *clist)
 *      cli_sock:    The server-side socket that is connected to the client
 *      clist:       The command_list_t structure that we implemented in
 *                   the last shell. 
 *   
 *  This function executes the command pipeline.  It should basically be a
 *  replica of the execute_pipeline() function from the last deliverable. 
 *  The only thing different is that you will be using the cli_sock as the
 *  main file descriptor on the first executable in the pipeline for STDIN,
 *  and the cli_sock for the file descriptor for STDOUT, and STDERR for the
 *  last executable in the pipeline.  See picture below:  
 * 
 *      
 *┌───────────┐                                                    ┌───────────┐
 *│ cli_sock  │                                                    │ cli_sock  │
 *└─────┬─────┘                                                    └────▲──▲───┘
 *      │   ┌──────────────┐     ┌──────────────┐     ┌──────────────┐  │  │    
 *      │   │   Process 1  │     │   Process 2  │     │   Process N  │  │  │    
 *      │   │              │     │              │     │              │  │  │    
 *      └───▶stdin   stdout├─┬──▶│stdin   stdout├─┬──▶│stdin   stdout├──┘  │    
 *          │              │ │   │              │ │   │              │     │    
 *          │        stderr├─┘   │        stderr├─┘   │        stderr├─────┘    
 *          └──────────────┘     └──────────────┘     └──────────────┘   
 *                                                      WEXITSTATUS()
 *                                                      of this last
 *                                                      process to get
 *                                                      the return code
 *                                                      for this function       
 * 
 *  Returns:
 * 
 *      EXIT_CODE:  This function returns the exit code of the last command
 *                  executed in the pipeline.  If only one command is executed
 *                  that value is returned.  Remember, use the WEXITSTATUS()
 *                  macro that we discussed during our fork/exec lecture to
 *                  get this value. 
 */
int rsh_execute_pipeline(int cli_sock, command_list_t *clist) {
    int pipes[clist->num - 1][2];  // Array of pipes
    pid_t pids[clist->num];
    int  pids_st[clist->num];         // Array to store process IDs
    Built_In_Cmds bi_cmd;
    int exit_code;
    int prev_pipe_fd = -1;

    // Create all necessary pipes
    for (int i = 0; i < clist->num - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < clist->num; i++) {
        // TODO this is basically the same as the piped fork/exec assignment, except for where you connect the begin and end of the pipeline (hint: cli_sock)
        // TODO HINT you can dup2(cli_sock with STDIN_FILENO, STDOUT_FILENO, etc.
        
        // Fork a child process.
        pids[i] = fork();

        if (pids[i] < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);

        } else if (pids[i] == 0)    // Child process
        {
            /* For STDIN */
            // Use cli_sock as the STDIN for first command.
            if (i == 0)
            {
                dup2(cli_sock, STDIN_FILENO);
            } else
            {
                // Use prev_pipe_fd as STDIN for later commands.
                dup2(prev_pipe_fd, STDIN_FILENO);
            }
            
            /* For STDOUT */
            // If not last command, then use write end of current pipe to direct the output.
            if (i < clist->num - 1)
            {
                dup2(pipes[i][1], STDOUT_FILENO);
            } else
            {
                // For last command, direct output to cli_sock.
                dup2(cli_sock, STDOUT_FILENO);
                dup2(cli_sock, STDERR_FILENO);
            }

            // Close unused descriptors.
            if (i > 0)
            {
                close(prev_pipe_fd);
            }

            // Execute the command/
            execvp(clist->commands[i].argv[0], clist->commands[i]. argv);
            perror("execvp");
            exit(EXIT_FAILURE);

        } else  // Parent Process
        {
            // Close the previous pipe.
            if (i > 0)
            {
                close(prev_pipe_fd);
            }
            // If it's not the last command, update the prev_pipe_fd and close the write end.
            if (i < clist->num - 1)
            {
                prev_pipe_fd = pipes[i][0];
                close(pipes[i][1]);
            }
        }
    }

    // Parent process: close all pipe ends
    for (int i = 0; i < clist->num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all children
    for (int i = 0; i < clist->num; i++) {
        waitpid(pids[i], &pids_st[i], 0);
    }

    //by default get exit code of last process
    //use this as the return value
    exit_code = WEXITSTATUS(pids_st[clist->num - 1]);
    for (int i = 0; i < clist->num; i++) {
        //if any commands in the pipeline are EXIT_SC
        //return that to enable the caller to react
        if (WEXITSTATUS(pids_st[i]) == EXIT_SC)
            exit_code = EXIT_SC;
    }
    return exit_code;
}

/**************   OPTIONAL STUFF  ***************/
/****
 **** NOTE THAT THE FUNCTIONS BELOW ALIGN TO HOW WE CRAFTED THE SOLUTION
 **** TO SEE IF A COMMAND WAS BUILT IN OR NOT.  YOU CAN USE A DIFFERENT
 **** STRATEGY IF YOU WANT.  IF YOU CHOOSE TO DO SO PLEASE REMOVE THESE
 **** FUNCTIONS AND THE PROTOTYPES FROM rshlib.h
 **** 
 */

/*
 * rsh_match_command(const char *input)
 *      cli_socket:  The string command for a built-in command, e.g., dragon,
 *                   cd, exit-server
 *   
 *  This optional function accepts a command string as input and returns
 *  one of the enumerated values from the BuiltInCmds enum as output. For
 *  example:
 * 
 *      Input             Output
 *      exit              BI_CMD_EXIT
 *      dragon            BI_CMD_DRAGON
 * 
 *  This function is entirely optional to implement if you want to handle
 *  processing built-in commands differently in your implementation. 
 * 
 *  Returns:
 * 
 *      BI_CMD_*:   If the command is built-in returns one of the enumeration
 *                  options, for example "cd" returns BI_CMD_CD
 * 
 *      BI_NOT_BI:  If the command is not "built-in" the BI_NOT_BI value is
 *                  returned. 
 */
Built_In_Cmds rsh_match_command(const char *input)
{
    if (strcmp(input, "exit") == 0)
        return BI_CMD_EXIT;
    if (strcmp(input, "dragon") == 0)
        return BI_CMD_DRAGON;
    if (strcmp(input, "cd") == 0)
        return BI_CMD_CD;
    if (strcmp(input, "stop-server") == 0)
        return BI_CMD_STOP_SVR;
    if (strcmp(input, "rc") == 0)
        return BI_CMD_RC;
    return BI_NOT_BI;
}

/*
 * rsh_built_in_cmd(cmd_buff_t *cmd)
 *      cmd:  The cmd_buff_t of the command, remember, this is the 
 *            parsed version fo the command
 *   
 *  This optional function accepts a parsed cmd and then checks to see if
 *  the cmd is built in or not.  It calls rsh_match_command to see if the 
 *  cmd is built in or not.  Note that rsh_match_command returns BI_NOT_BI
 *  if the command is not built in. If the command is built in this function
 *  uses a switch statement to handle execution if appropriate.   
 * 
 *  Again, using this function is entirely optional if you are using a different
 *  strategy to handle built-in commands.  
 * 
 *  Returns:
 * 
 *      BI_NOT_BI:   Indicates that the cmd provided as input is not built
 *                   in so it should be sent to your fork/exec logic
 *      BI_EXECUTED: Indicates that this function handled the direct execution
 *                   of the command and there is nothing else to do, consider
 *                   it executed.  For example the cmd of "cd" gets the value of
 *                   BI_CMD_CD from rsh_match_command().  It then makes the libc
 *                   call to chdir(cmd->argv[1]); and finally returns BI_EXECUTED
 *      BI_CMD_*     Indicates that a built-in command was matched and the caller
 *                   is responsible for executing it.  For example if this function
 *                   returns BI_CMD_STOP_SVR the caller of this function is
 *                   responsible for stopping the server.  If BI_CMD_EXIT is returned
 *                   the caller is responsible for closing the client connection.
 * 
 *   AGAIN - THIS IS TOTALLY OPTIONAL IF YOU HAVE OR WANT TO HANDLE BUILT-IN
 *   COMMANDS DIFFERENTLY. 
 */
Built_In_Cmds rsh_built_in_cmd(cmd_buff_t *cmd)
{
    Built_In_Cmds ctype = BI_NOT_BI;
    ctype = rsh_match_command(cmd->argv[0]);

    switch (ctype)
    {
    // case BI_CMD_DRAGON:
    //     print_dragon();
    //     return BI_EXECUTED;
    case BI_CMD_EXIT:
        return BI_CMD_EXIT;
    case BI_CMD_STOP_SVR:
        return BI_CMD_STOP_SVR;
    case BI_CMD_RC:
        return BI_CMD_RC;
    case BI_CMD_CD:
        chdir(cmd->argv[1]);
        return BI_EXECUTED;
    default:
        return BI_NOT_BI;
    }
}
