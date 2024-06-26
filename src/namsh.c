#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include "../headers/builtin.h"

#define NAMSH_RL_BUFSIZE 1024 // readline buffer
#define NAMSH_TOK_BUFSIZE 64 // token buffer
#define NAMSH_TOK_DELIM " \t\r\n\a" // delimiters 

// ANSI color codes
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define PURPLE "\033[0;35m"
#define ANSI_BOLD "\033[1m"
#define ANSI_UNDERLINE "\033[4m"
#define ANSI_RESET "\033[0m"

int namsh_launch(char** args)
{
    pid_t pid, wpid;
    int status;

    pid = fork(); // returns 0 to child process, returns pid of child to parent process

    if (pid == 0) 
    {
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

int namsh_execute(char** args)
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
            return (*builtin_func[i])(args); // calling the function
        }
    }

    // if function is not a builtin function
    return namsh_launch(args);
}

char* namsh_read_line()
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

char** namsh_split_line(char* line)
{
    int bufsize = NAMSH_TOK_BUFSIZE;
    int position = 0;
    char** tokens = malloc(sizeof(char*) * bufsize);
    char* token;

    if (!tokens) 
    {
        fprintf(stderr, "namsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, NAMSH_TOK_DELIM);

    while (token != NULL) 
    {
        tokens[position] = token;
        position++;

        if (position >= bufsize) 
        {
            bufsize += NAMSH_TOK_BUFSIZE;
            tokens = realloc(tokens, sizeof(char*) * bufsize);

            if (!tokens) 
            {
                fprintf(stderr, "namsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, NAMSH_TOK_DELIM); // continue tokenizing the string
    }

    tokens[position] = NULL;
    return tokens;
}

void print_prompt()
{
    struct passwd* pw; // info about user account
    char cwd[1024];
    char* relative_cwd;
    char* home;
    char* username;
    char* current_dir;
    char relative_path[1024];

    // get cwd
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        // get username and home directory
        pw = getpwuid(getuid());
        username = pw->pw_name;
        home = pw->pw_dir;

        // get last directory name (current directory)
        current_dir = strrchr(cwd, '/');
        if (current_dir != NULL)
            current_dir++; // remove the /
        else
            current_dir = cwd; // root directory

        // check if cwd starts with home
        if (strncmp(cwd, home, strlen(home)) == 0)
        {
                // create the relative path (by advancing the pointer)
                relative_cwd = cwd + strlen(home);

                if (relative_cwd[0] == '\0')
                {
                    relative_cwd = "/";
                }
                else // create relative path
                {
                    strncpy(relative_path, relative_cwd, current_dir - relative_cwd);
                    relative_path[current_dir - relative_cwd] = '\0';
                }

                printf("%s%s%s%s@%s%s%s%s%s> ", GREEN, ANSI_BOLD, username, PURPLE, BLUE, relative_path, BLUE, ANSI_BOLD, current_dir);
        }
        else
        {
            printf("%s%s%s%s@%s%s%s%s%s> ", GREEN, ANSI_BOLD, username, PURPLE, BLUE, relative_cwd, BLUE, ANSI_BOLD, current_dir);
        }

        printf(ANSI_RESET);
    }
    else
    {
        fprintf(stderr, "namsh: cannot get cwd");
    }
}

void namsh_loop()
{
    char* line;
    char** args;
    int status;

    do 
    {
        print_prompt();

        line = namsh_read_line();
        args = namsh_split_line(line);
        status = namsh_execute(args);

        free(line);
        free(args);
    } while (status);
}

int main(int argc, char** argv)
{
    // main program loop
    namsh_loop();

    // TODO: shut down and clean up the shell

    return EXIT_SUCCESS;
}
