1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  The main reason we use fork before execvp is to create a separate process to execute the new program. If we called execvp directly in the current process, it would replace our shell or parent process entirely, which isn't what we want. 

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If fork() fails it usually means the system is out of resources. In my implementation I check the return value of fork(). If it's -1, I print an error message and return an appropriate error code.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**: execvp() searches for the command in directories listed in the PATH environment variable. If the command is just a name execvp() checks each directory in PATH until it finds an executable file with that name. 
4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  Calling wait() ensures that the parent process pauses until the child process finishes execution. If we dont call wait(), the child process' exit status remains in the process table until the parent reads it.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**: WEXITSTATUS() extracts the exit status of a terminated child from the value returned by wait(). This is important because it allows the parent to determine whether the child executed successfully or encountered an error, which can help in handling errors.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**: My implementation of build_cmd_buff() properly identifies quoted arguments so that spaces inside quotes aren’t treated as argument separators. This is necessary to allow multi-word argumentsto be passed correctly as a single argument instead of being split into multiple ones.
7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**: I modified the parsing logic to better handle special characters and quoted arguments, making it more flexible. One unexpected challenge was ensuring that words outside the quotes were also  handled correctly

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals are a form of asynchronous communication between processes and the OS, mainly used to notify a process of events like termination requests or interrupts. Unlike pipes,signals just send a simple notification.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:
    -SIGKILL (9): Immediately terminates a process and cannot be ignored or caught. Used when a process is unresponsive.
-SIGTERM (15): Politely asks a process to terminate, allowing it to clean up before exiting. Used for safe shutdowns.
-SIGINT (2): Sent when the user presses Ctrl+C to interrupt a process running in the terminal. Useful for stopping programs interactively.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**: When a process receives SIGSTOP, it is immediately paused by the OS until it receives SIGCONT to resume. it cannot be caught or ignored so that even misbehaving processes can be forcibly stopped if needed.
