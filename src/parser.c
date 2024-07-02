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
    size_t bufsize = RL_BUFSIZE;
    char* buffer = malloc(bufsize);

    if (!buffer)
    {
        fprintf(stderr, "namsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    size_t index = 0;
    char c;
    size_t cursor = 0; // cursor position in the line

    enable_raw_mode();

    while (1)
    {
        c = getchar();

        if (c == '\033') // escape character for arrow keys
        {
            getchar(); // skip the '['

            switch (getchar()) // evaluate which key it is
            {
                case 'D': // left arrow
                    if (cursor > 0)
                    {
                        putchar('\033');
                        putchar('[');
                        putchar('D');
                        cursor--;
                    }
                    break;
                
                case 'C': // right arrow
                    if (cursor < index)
                    {
                        putchar('\033');
                        putchar('[');
                        putchar('C');
                        cursor++;
                    }
                    break;
            }
        }
        else if (c == '\n') // newline
        {
            buffer[index] = '\0';
            putchar('\n');
            break;
        }
        else if (c == '\t') // tab
        {
            if (index + 4 > bufsize)
            {
                bufsize += RL_BUFSIZE;
                buffer = realloc(buffer, bufsize);

                if (!buffer)
                {
                    fprintf(stderr, "namsh: allocation error\n");
                    exit(EXIT_FAILURE);
                }
            }

            for (int i = 0; i < 4; i++)
            {
                buffer[index++] = ' ';
                putchar(' ');
            }
            cursor += 4;
        }
        else if (c == 127) // backspace
        {
            if (index > 0)
            {
                if (index >= 4 && strncmp(&buffer[index - 4], "    ", 4) == 0)
                {
                    index -= 4;
                    cursor -= 4;

                    for (int i = 0; i < 4; i++)
                    {
                        // have to do this cuz \b just move the cursor back, it doesn't remove the character
                        putchar('\b');
                        putchar(' ');
                        putchar('\b');
                    }
                }
                else
                {
                    index--;
                    cursor--;
                    putchar('\b');
                    putchar(' ');
                    putchar('\b');
                }
            }
        }
        else // add character
        {
            if (index > bufsize)
            {
                bufsize += RL_BUFSIZE;
                buffer = realloc(buffer, bufsize);

                if (!buffer)
                {
                    fprintf(stderr, "namsh: allocation error\n");
                    exit(EXIT_FAILURE);
                }
            }

            buffer[index++] = c;
            cursor++;
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
