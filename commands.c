#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

/* builtin names (note: kept name builtin_srt to match existing code) */
char *builtin_srt[] = {
    "cd",
    "help",
    "exit",
    "read",   /* read file */
    "touch",  /* create file */
    "mkdir",  /* create directory */
    "write"   /* write to file (overwrite) */
};

int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit,
    &lsh_readfile,
    &lsh_touch,
    &lsh_mkdir,
    &lsh_write
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

/* New commands */

/* read file and print to stdout */
int lsh_readfile(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Usage: read <filename>\n");
        return 1;
    }

    FILE *f = fopen(args[1], "r");
    if (!f) {
        perror("read");
        return 1;
    }

    char buf[1024];
    while (fgets(buf, sizeof(buf), f)) {
        fputs(buf, stdout);
    }
    fclose(f);
    fflush(stdout);
    return 1;
}

/* create empty file (or update timestamp) */
int lsh_touch(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Usage: touch <filename>\n");
        return 1;
    }

    FILE *f = fopen(args[1], "a");
    if (!f) {
        perror("touch");
        return 1;
    }
    fclose(f);
    return 1;
}

/* create directory */
int lsh_mkdir(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Usage: mkdir <directory>\n");
        return 1;
    }

    if (mkdir(args[1], 0755) != 0) {
        perror("mkdir");
    }
    return 1;
}

/* write text to file (overwrites). Usage: write <filename> <text...> */
int lsh_write(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "Usage: write <filename> <text>\n");
        return 1;
    }

    FILE *f = fopen(args[1], "w");
    if (!f) {
        perror("write");
        return 1;
    }

    /* join remaining args with spaces */
    for (int i = 2; args[i] != NULL; ++i) {
        if (i > 2) fputc(' ', f);
        fputs(args[i], f);
    }
    fputc('\n', f);
    fclose(f);
    return 1;
}