#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *builtin_srt[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit
};  

int lsh_num_builtins() {
    return sizeof(builtin_srt) / sizeof(char *);
}

int lsh_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("lsh");
        }
    }
    return 1;
}

int lsh_help(char **args) {
    int i;
    printf("C Shell\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (i = 0; i < lsh_num_builtins(); i++) {
        printf("  %s\n", builtin_srt[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int lsh_exit(char **args) {
    /* Signal to the Python GUI that the shell is exiting and terminate the process.
       The GUI can watch for the sentinel "###CSHELL_EXIT###" or detect EOF and then
       close its window. */
    printf("###CSHELL_EXIT###\n");
    fflush(stdout);
    fflush(stderr);
    exit(EXIT_SUCCESS); /* does not return */
    return 0; /* unreachable, keeps signature happy if compiled with warnings */
}