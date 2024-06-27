#ifndef PARSER_H
#define PARSER_H

// token constants
#define NAMSH_RL_BUFSIZE 1024 // readline buffer
#define NAMSH_TOK_BUFSIZE 64 // token buffer
#define NAMSH_TOK_DELIM " \t\r\n\a" // delimiters 

int namsh_launch(char** args);
int namsh_execute(char** args);
char* namsh_read_line();
char** namsh_split_line(char* line);

#endif
