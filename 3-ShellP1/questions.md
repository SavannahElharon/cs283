1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**: fgets() is a good choice for this assignment because it handles special characters and  prevents buffer overflow and since we are making a shell that takes commands the input this functionality of fgets is very efficient.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  We used malloc because it is much more flexible to allocate only the amount of memory needed for input.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**: Trimming leading and trailing spaces ensures that commands are correctly parsed and executed despite the extra spaces. If we didn’t remove extra spaces, we might encounter issues such as incorrect tokenization, unexpected empty commands, or problems with matching input to the functionality.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  Three redirection examples are output (>), input (<), and error (2>) redirection. Some challenges we might face when implementing them is parsing the command with the sign correctly and being able to execute.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection changes where a command reads input from or writes output to while piping connects the output of one command directly to the input of another, allowing command chaining.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  Keeping STDOUT and STDERR separate allows error messages to be processed independently from normal output. This is important so that the debugging and logging is effient and communicated to the user correctly.
- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**: Our shell should capture the exit status of commands and provide specific error messages when a command fails. It should also allow users to merge STDOUT and STDERR so that when users want all output in a single stream they can. However, we should still provide an option to keep them separate when needed for debugging purposes.
