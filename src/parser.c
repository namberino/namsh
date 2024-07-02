#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "parser.h"
#include "builtin.h"

void enable_raw_mode(void)
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void disable_raw_mode(void)
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

int launch_child_process(char** args)
{
    pid_t pid;
    // pid_t wpid;
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
            // wpid = waitpid(pid, &status, WUNTRACED);
            waitpid(pid, &status, WUNTRACED);
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

char* shell_readline(void)
{
    size_t bufsize = 1024;
    char* buffer = malloc(bufsize);

    if (!buffer)
    {
        fprintf(stderr, "namsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    size_t pos = 0;
    char c;

    enable_raw_mode();

    while (1)
    {
        c = getchar();

        if (c == '\n')
        {
            buffer[pos] = '\0';
            putchar('\n');
            break;
        }
        else if (c == '\t')
        {
            // tab key handling: insert four spaces
            if (pos + 4 >= bufsize)
            {
                bufsize += 1024;
                buffer = realloc(buffer, bufsize);
                
                if (!buffer)
                {
                    fprintf(stderr, "namsh: allocation error\n");
                    exit(EXIT_FAILURE);
                }
            }

            for (int i = 0; i < 4; i++)
            {
                buffer[pos++] = ' ';
                putchar(' ');
            }
        }
        else if (c == 127)
        {  // backspace handling (ASCII 127)
            if (pos > 0)
            {
                if (pos >= 4 && strncmp(&buffer[pos - 4], "    ", 4) == 0)
                {
                    pos -= 4;

                    for (int i = 0; i < 4; i++)
                    {
                        putchar('\b');
                    }
                }
                else
                {
                    pos--;

                    putchar('\b');
                }
            }
        }
        else
        {
            if (pos >= bufsize)
            {
                bufsize += 1024;
                buffer = realloc(buffer, bufsize);

                if (!buffer)
                {
                    fprintf(stderr, "namsh: allocation error\n");
                    exit(EXIT_FAILURE);
                }
            }

            buffer[pos++] = c;
            putchar(c);
        }
    }

    disable_raw_mode();
    
    return buffer;
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
