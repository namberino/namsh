# Shell loop

A shell does 3 things:
- **Initialize**: Read and execute config files.
- **Intepret**: Read commands from *stdin* (from interactive shell or from file) and executes them
- **Terminate**: Executes shutdown commands, frees up memories, and terminates

# Command handling

- **Read**: Read command from stdin
- **Parse**: Separate command string from command arguments
- **Execute**: Run parsed command

# Process handling

When a process starts, the existing process forks (`fork()` syscall) itself into 2 (a parent process and a child process). The child process uses `exec()` syscall to replace itself with a new program. The parent process continues doing other things, it can also keep tabs on its children (`wait()` syscall)
