#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

#include "../headers/builtin.h"
#include "../headers/parser.h"
#include "../headers/util.h"

void shell_loop()
{
    char* line;
    char** args;
    int status;

    do 
    {
        print_prompt();

        line = shell_readline();
        args = shell_splitline(line);
        status = execute_cmd(args);

        free(line);
        free(args);
    } while (status);
}

int main(int argc, char** argv)
{
    // main program loop
    shell_loop();

    // TODO: shut down and clean up the shell

    return EXIT_SUCCESS;
}
