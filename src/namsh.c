#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <pwd.h>

#include "../headers/ansi.h"
#include "../headers/builtin.h"
#include "../headers/parser.h"

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
