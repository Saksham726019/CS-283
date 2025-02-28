1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

The shell waits for each child process by calling waitpid() in a loop after forking all piped commands. This makes sure that the shell does not accept new input until all child processes have finished. Without waitpid(), there will be zombie processes and system resources will be wasted.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

It is necessary to close them becasue dup2() duplicates the file descriptor from the original pipe and closing must be done inorder to save system resources. If they are left open, there will be file descriptor leaks.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

cd is implemented as built-in command because cd must be run in the current process rather than the child process. This ensures that the directory changes in the shell.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

To allow arbitary number of piped commands, I would use dyamic memory allocation with malloc and use realloc to allocate more memonry in case the piped command increases more. The trade-off is that malloc and realloc increases overhead, leading to slow execution and also they need to be carefully freed, otherwise could lead to memory leaks.
