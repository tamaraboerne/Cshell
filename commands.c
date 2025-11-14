#include "commands.h"

char *builtin_srt[] = {
    "cd",
    "help",
    "exit",
    "read"
};

int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit,
    &lsh_read
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
    printf("Shell\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (i = 0; i < lsh_num_builtins(); i++) {
        printf("  %s\n", builtin_srt[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int lsh_exit(char **args) {
    return 0;
}

int lsh_read(char **args) {
    //function to read a file line by line and print to standard output
    //write me this code using getline
    if (args[1] == NULL) {
        fprintf(stderr, "lsh: expected argument to \"read\"\n");
        return 1;
    }   
    FILE *file = fopen(args[1], "r");
    if (file == NULL) {
        perror("lsh: could not open file");
        return 1;
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, file)) != -1) {
        printf("%s", line);
    }
    printf("\n");
    free(line);
    fclose(file);
    return 1;        
}