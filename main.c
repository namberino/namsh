#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NAMSH_RL_BUFSIZE 1024 // readline buffer
#define NAMSH_TOK_BUFSIZE 64
#define NAMSH_TOK_DELIM " \t\r\n\a"

int namsh_cd(char** args);
int namsh_help(char** args);
int namsh_exit(char** args);

char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[])(char**) = {
    &namsh_cd,
    &namsh_help,
    &namsh_exit
};

int namsh_num_builtins() 
{
    return sizeof(builtin_str) / sizeof(char *);
}

int namsh_cd(char** args)
{
    if (args[1] == NULL) 
    {
        fprintf(stderr, "namsh: expected argument to \"cd\"\n");
    } 
    else 
    {
        if (chdir(args[1]) != 0) 
        {
            perror("namsh");
        }
    }

    return 1;
}

int namsh_help(char** args)
{
    int i;

    printf("Nam's namsh\n\n");
    printf("The followings are built in:\n");

    for (i = 0; i < namsh_num_builtins(); i++) 
    {
        printf("  %s\n", builtin_str[i]);
    }

    printf("\nUse the man command for information on other programs.\n");

    return 1;
}

int namsh_exit(char** args)
{
    return 0;
}


int namsh_launch(char** args)
{
    pid_t pid, wpid;
    int status;
    pid = fork();

    if (pid == 0) 
    {
        // Child process
        if (execvp(args[0], args) == -1) 
        {
            perror("namsh");
        }
        
        exit(EXIT_FAILURE);
    } 
    else if (pid < 0) 
    {
        // Error forking
        perror("namsh");
    } 
    else 
    {
        // Parent process
        do 
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int namsh_execute(char** args)
{
    int i;

    // if n empty command was entered
    if (args[0] == NULL) 
    {
        return 1;
    }

    for (i = 0; i < namsh_num_builtins(); i++) 
    {
        if (strcmp(args[0], builtin_str[i]) == 0) 
        {
            return (*builtin_func[i])(args);
        }
    }

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
    int bufsize = NAMSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

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
            tokens = realloc(tokens, bufsize * sizeof(char*));

            if (!tokens) 
            {
                fprintf(stderr, "namsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, NAMSH_TOK_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}

void namsh_loop()
{
    char* line;
    char** args;
    int status;

    do 
    {
        printf("> ");

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
