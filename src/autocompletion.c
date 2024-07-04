#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

#include "ansi.h"
#include "builtin.h"

char** get_matching_commands(char* cmd, int* n)
{
    int bufsize = 100;
    int index = 0;
    char** matches = malloc(bufsize * sizeof(char*));
    *n = 0; // number of matches

    if (!matches)
    {
        fprintf(stderr, "namsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < (sizeof(builtin_str) / sizeof(char*)); i++) 
    {
        if (strncmp(cmd, builtin_str[i], strlen(cmd)) == 0) 
        {
            matches[index++] = strdup(builtin_str[i]);

            if (index >= bufsize) 
            {
                bufsize += 100;
                matches = realloc(matches, bufsize * sizeof(char*));

                if (!matches) 
                {
                    fprintf(stderr, "namsh: allocation error\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    *n = index;

    return matches;
}

void display_matches(char** matches, int n)
{
    printf("\n");
    for (int i = 0; i < n; i++)
        printf("\t%s%s%s", ANSI_BOLD, matches[i], ANSI_RESET);
}
