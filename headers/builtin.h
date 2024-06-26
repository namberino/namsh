#ifndef BUILTIN_H
#define BUILTIN_H

int namsh_cd(char** args);
int namsh_help(char** args);
int namsh_exit(char** args);

extern char* builtin_str[3];

// array of function pointers (that can take in an array of strings, which are args, and returns an int)
extern int (*builtin_func[])(char**);

#endif
