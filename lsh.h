#ifndef LSH_H
#define LSH_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


//defines 
#define LSH_BUFSIZE 1024;
#define LSH_TOK_BUFSIZE 64;
#define LSH_TOK_DELIM " \t\r\n\a";

//functions
void lsh_loop();
char *lsh_read_line();
char **lsh_split_line(char *line);
int lsh_execute(char **args);


#endif 