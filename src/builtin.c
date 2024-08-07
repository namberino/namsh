#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"
#include "ansi.h"

char* builtin_str[4] = {
    "cd",
    "help",
    "exit",
    "history"
};

int (*builtin_func[])(char**) = {
    &cmd_cd,
    &cmd_help,
    &cmd_exit,
    &cmd_history
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
    printf("Welcome to %s%snamsh%s, the minimalist shell.\n", ANSI_BOLD, BLUE, ANSI_RESET);
    printf("%s _   _   ___  ___  ___ _____ _   _ \n", CYAN);
    printf("| \\ | | / _ \\ |  \\/  |/  ___| | | |\n");
    printf("|  \\| |/ /_\\ \\| .  . |\\ `--.| |_| |\n");
    printf("| . ` ||  _  || |\\/| | `--. \\  _  |\n");
    printf("| |\\  || | | || |  | |/\\__/ / | | |\n");
    printf("\\_| \\_/\\_| |_/\\_|  |_/\\____/\\_| |_/\n\n%s", ANSI_RESET);
    printf("The following commands are built into the shell:\n");

    for (int i = 0; i < (sizeof(builtin_str) / sizeof(char*)); i++) 
    {
        printf("%s\t%s\n", GREEN, builtin_str[i]);
    }

    printf("%s\nUse the %s%sman%s command to get information on other commands.\n", ANSI_RESET, ANSI_BOLD, ANSI_UNDERLINE, ANSI_RESET);

    return 1;
}

int cmd_exit(char** args)
{
    return 0;
}

int cmd_history(char** args)
{
    char* home_dir = getenv("HOME");

    if (home_dir == NULL)
        fprintf(stderr, "namsh: HOME environment variable not set");

    char history_path[1024];
    snprintf(history_path, sizeof(history_path), "%s/%s", home_dir, HISTORY_FILE);

    FILE* history_file = fopen(history_path, "r");
    if (history_file == NULL)
        perror("namsh: Error opening history file");

    char line_buffer[1024];
    
    while (fgets(line_buffer, sizeof(line_buffer), history_file) != NULL)
        printf("%s", line_buffer);

    fclose(history_file);

    return 1;
}

void log_history(char** args)
{
    char* home_dir = getenv("HOME");

    if (home_dir == NULL)
        fprintf(stderr, "Error: HOME environment variable not set.\n");

    char history_path[1024];
    snprintf(history_path, sizeof(history_path), "%s/%s", home_dir, HISTORY_FILE);

    // open the file in read mode to get the last id
    FILE* history_file = fopen(history_path, "r");
    int last_id = 0;

    if (history_file != NULL)
    {
        char line[1024];
        while (fgets(line, sizeof(line), history_file) != NULL)
        {
            char* colon_pos = strchr(line, ':');

            if (colon_pos != NULL)
                last_id = atoi(line);
        }
        fclose(history_file);
    }

    // increment the id for the new command
    int cmd_id = ++last_id;

    // writing the command into the history file
    history_file = fopen(history_path, "a");
    if (history_file == NULL)
        perror("namsh: Error opening history file");

    // concatenate all arguments into a single command string
    char command[1024] = "";
    for (int i = 0; args[i] != NULL; i++)
    {
        strcat(command, args[i]);
        strcat(command, " ");
    }

    fprintf(history_file, "%d:%s\n", cmd_id, command);

    fclose(history_file);
}
