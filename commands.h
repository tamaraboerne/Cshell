#ifndef COMMANDS_H
#define COMMANDS_H

/* External declarations */
extern char *builtin_srt[];
extern int (*builtin_func[])(char **);

/* Builtin function prototypes */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_num_builtins(void);

/* New file/directory commands */
int lsh_readfile(char **args); /* read and print file contents */
int lsh_touch(char **args);    /* create empty file or update timestamp */
int lsh_mkdir(char **args);    /* create directory */
int lsh_write(char **args);    /* write text to file (overwrites) */

#endif /* COMMANDS_H */