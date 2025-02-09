1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  fgets() is a good choice because it can handle inputs with spaces and only stops when it detects newline \n or EOF. Especially in the test cases in test.sh, EOF is used to indicate end of list of commands passed to the program.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**: Using malloc was more helpful as it allows for more flexible and dynamic memory management rather than relying on a fixed stack memory. For example, we could reallocate memory in order to allow more input, but if we used stack memory for cmd_buffer, it doesn't allow much felxibility.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Trimming spaces is necessary because we don't want the shell to think space is also a command, which could cause incorrect execution. For example, if 'cmd1' is supposed to execute something. But when programming the use of 'cmd1', we did not remove leading and trailing spaces. That would mean, we need to input ' cmd' to shell, which is incorrect.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**: The three redirecitions:
    * Input redirection (<): Takes input from a file instead of the keyboard.
    * Output redirection (>) or (>>): Redirects the output to a file instead of the window or screen.
    * Error redirection (2>) or (2>>): Sends error messages (STDERR) to a file instead of displaying them on the screen.

    The challenges we might have are overwriting the existing file. Thus, we need to be careful and use >> for appending. Moreover, we also need to careful about file permissions as read only file can't be used for output redirection.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection sends input, output, or errors to or from a file. Piping connects the output of one command directly to the input of another without using files.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**: It is important to keep STDOUT and STDERR separate in a shell because it will help keep things clean as it will separate the output of the program from the error of the program, which allows for easness in debugging.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**: Our shell should handle if a command fails by the use of the exit status. We can provide a way to merge both STDOUT and STDERR by using 1> for STDOUT and 2> for STDERR. 