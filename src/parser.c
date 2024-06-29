#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>

#include "../headers/parser.h"
#include "../headers/builtin.h"

int launch_child_process(char** args)
{
    pid_t pid, wpid;
    int status;

    pid = fork(); // returns 0 to child process, returns pid of child to parent process

    if (pid == 0) 
    {
        log_history(args);

        // Child process
        if (execvp(args[0], args) == -1) // 1st one is the program name
        {
            fprintf(stderr, "namsh: command not found\n");
        }
        
        exit(EXIT_FAILURE);
    } 
    else if (pid < 0) 
    {
        // forking error
        fprintf(stderr, "namsh: cannot create new process\n");
    } 
    else 
    {
        // parent process (wait for process to finish)
        do 
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status)); // wait till process is exited or killed
    }

    return 1; // signals the shell to prompt for input again
}

int execute_cmd(char** args)
{
    // if an empty command was entered
    if (args[0] == NULL) 
    {
        return 1;
    }

    for (int i = 0; i < (sizeof(builtin_str) / sizeof(char*)); i++) 
    {
        if (strcmp(args[0], builtin_str[i]) == 0) 
        {
            log_history(args);
            return (*builtin_func[i])(args); // calling the function
        }
    }

    // if function is not a builtin function
    return launch_child_process(args);
}

char* shell_readline()
{
    char* line = NULL;
    size_t bufsize = 0;

    if (getline(&line, &bufsize, stdin) == -1)
    {
        if (feof(stdin))
        {
            exit(EXIT_SUCCESS);
        } 
        else 
        {
            fprintf(stderr, "namsh: readline error\n");
            exit(EXIT_FAILURE);
        }
    }

    return line;
}

char** shell_splitline(char* line)
{
    int bufsize = TOKEN_BUFSIZE;
    int position = 0;
    char** tokens = malloc(sizeof(char*) * bufsize);
    char* token;

    if (!tokens) 
    {
        fprintf(stderr, "namsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOKEN_DELIMS);

    while (token != NULL) 
    {
        tokens[position] = token;
        position++;

        if (position >= bufsize) 
        {
            bufsize += TOKEN_BUFSIZE;
            tokens = realloc(tokens, sizeof(char*) * bufsize);

            if (!tokens) 
            {
                fprintf(stderr, "namsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, TOKEN_DELIMS); // continue tokenizing the string
    }

    tokens[position] = NULL;
    return tokens;
}
