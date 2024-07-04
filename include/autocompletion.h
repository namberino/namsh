#ifndef AUTOCOMPLETION_H
#define AUTOCOMPLETION_H

char** get_matching_commands(char* cmd, int* n);
void display_matches(char** matches, int n);

#endif
