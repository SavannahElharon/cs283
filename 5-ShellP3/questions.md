1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

My implementation uses waitpid in a loop to wait for all child processes to complete before continuing. If I forgot to call waitpid some child processes might become "zombies," consuming system resources and causing unpredictable behavior.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

After calling dup2() unused pipe ends must be closed to avoid resource leaks and ensure proper termination of child processes. Leaving pipes open could cause deadlocks or lead to system errors.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

cd is implemented as a built-in because it needs to change the shell's current directory directly which isn't possible in a child process. If cd were an external command, it would only change the directory for the child process, not the parent shell.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

To allow an arbitrary number of piped commands, I would dynamically allocate memory for pipes and command buffers. The trade-off is increased complexity and potential memory overhead, but it provides flexibility for longer pipelines.
