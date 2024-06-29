#ifndef BUILTIN_H
#define BUILTIN_H

#define HISTORY_FILE ".namsh_history"

int cmd_cd(char** args);
int cmd_help(char** args);
int cmd_exit(char** args);
int cmd_history(char** args);

void log_history(char** args);

extern char* builtin_str[4];

// array of function pointers (that can take in an array of strings, which are args, and returns an int)
extern int (*builtin_func[])(char**);

#endif
