#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../headers/builtin.h"

char* builtin_str[3] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[])(char**) = {
    &cmd_cd,
    &cmd_help,
    &cmd_exit
};

int cmd_cd(char** args)
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

int cmd_help(char** args)
{
    printf("Nam's namsh\n\n");
    printf("The followings are built in:\n");

    for (int i = 0; i < (sizeof(builtin_str) / sizeof(char*)); i++) 
    {
        printf("  %s\n", builtin_str[i]);
    }

    printf("\nUse the \"man\" command for information on other programs\n");

    return 1;
}

int cmd_exit(char** args)
{
    return 0;
}

void log_history(char** args)
{
    char* home_dir = getenv("HOME");

    if (home_dir == NULL)
        fprintf(stderr, "Error: HOME environment variable not set.\n");

    char history_path[1024];
    snprintf(history_path, sizeof(history_path), "%s/%s", home_dir, HISTORY_FILE);

    FILE* history_file = fopen(history_path, "a");
    if (history_file == NULL)
        perror("namsh: Error opening history file");

    // concatenate all arguments into a single command string
    char command[1024] = "";
    for (int i = 0; args[i] != NULL; i++)
    {
        strcat(command, args[i]);
        strcat(command, " ");
    }

    fprintf(history_file, "%s\n", command);

    fclose(history_file);
}
