#ifndef COMMANDS_H
#define COMMANDS_H

/* External declarations */
extern char *builtin_srt[];
extern int (*builtin_func[])(char **);

/* Function prototypes */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_num_builtins(void);

#endif /* COMMANDS_H */