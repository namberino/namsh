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
    &namsh_cd,
    &namsh_help,
    &namsh_exit
};

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
    printf("Nam's namsh\n\n");
    printf("The followings are built in:\n");

    for (int i = 0; i < (sizeof(builtin_str) / sizeof(char*)); i++) 
    {
        printf("  %s\n", builtin_str[i]);
    }

    printf("\nUse the \"man\" command for information on other programs\n");

    return 1;
}

int namsh_exit(char** args)
{
    return 0;
}
