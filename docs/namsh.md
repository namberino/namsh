# Shell loop

A shell does 3 things:
- **Initialize**: Read and execute config files.
- **Intepret**: Read commands from *stdin* (from interactive shell or from file) and executes them
- **Terminate**: Executes shutdown commands, frees up memories, and terminates

# Command handling

- **Read**: Read command from stdin
- **Parse**: Separate command string from command arguments
- **Execute**: Run parsed command
