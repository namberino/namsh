#ifndef PARSER_H
#define PARSER_H

// token constants
#define RL_BUFSIZE 1024 // readline buffer
#define TOKEN_BUFSIZE 64 // token buffer
#define TOKEN_DELIMS " \t\r\n\a" // delimiters 

int launch_child_process(char** args);
int execute_cmd(char** args);
char* shell_readline(void);
char** shell_splitline(char* line);

#endif
