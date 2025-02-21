1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**: We use fork before execvp so that we create a new process for running the command, and thus, the original shell process intact. The execvp replaces the current process with the new one if it succeeds. So without fork we'd lose the shell process

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**: If fork() call fails, then it will return a negative value and my program will print "fork failed". Then, we return the error code. This way, we prevent the shell from crashing.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**: execvp() searches for the command in the PATH environment variable. So execvp() uses it to locate the command we want to run.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**: Wait will make the parent process pauses until the child process finishes execution. If we didn't call wait, the child process might remain as a zombie process.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**: WEXITSTATUS() extracts the exit code of the child process from the status returned by wait(). This exit code tells us whether the command ran successfully or not.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**: I use a boolean inside_quotes and initialize it as false. When we hit a quote, I do !inside_quotes, which in the starting makes it true. When we hit the ending quotes, the same !inside_quotes will make it false, marking it as the end of quotes. When iside_quotes is true, we ensure that spaces inside them are preserved as part of the same argument. It's necessary because whatever is inside quotes will be treated as a single command for commands like echo.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**: For this assignment, the main parsing changes were: removing the pipe handling, switching from command list to cmd buffer, and logic to parsing quoted strings.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**: Signals are used to notify processes of events like interruptions or termination requests, such as ctrl + c.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**: SIGKILL: Immediately terminates a process. SIGTERM: Requests a process to terminate. SIGINT: Triggered by pressing Ctrl+ C in the terminal, it interrupts a process.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**: SIGSTOP, pauses the process and stops executing until a SIGCONT signal resumes it. SIGSTOP cannot be caught or ignored, ensuring that processes can always be paused when necessary.
