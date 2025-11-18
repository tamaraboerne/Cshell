#include "lsh.h"
#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void lsh_loop() {
    char *line;
    char **args;
    int status;

    do {
        // Read line from stdin (Python GUI provides it)
        line = lsh_read_line();
        
        if (line == NULL || strlen(line) == 0) {
            free(line);
            continue;
        }
        
        // Parse line
        args = lsh_split_line(line);
        
        // Execute command
        status = lsh_execute(args);
        
        // Flush output so Python GUI sees it immediately
        fflush(stdout);
        fflush(stderr);
        
        // Free memory
        free(line);
        free(args);
        
    } while (status);
}

int lsh_launch(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("lsh");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

char *lsh_read_line() {
    char *line = NULL;
    size_t bufsize = 0;
    
    if (getline(&line, &bufsize, stdin) == -1) {
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);
        } else {
            perror("lsh: getline");
            exit(EXIT_FAILURE);
        }
    }
    
    // Remove newline
    if (line && line[strlen(line) - 1] == '\n') {
        line[strlen(line) - 1] = '\0';
    }
    
    return line;
}

char **lsh_split_line(char *line) {
    int bufsize = LSH_TOK_BUFSIZE; 
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token, *line_copy;

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    // strtok modifies the string, so make a copy
    line_copy = malloc(strlen(line) + 1);
    strcpy(line_copy, line);

    token = strtok(line_copy, " \t\r\n");
    while (token != NULL) {
        tokens[position] = malloc(strlen(token) + 1);
        strcpy(tokens[position], token);
        position++;

        if (position >= bufsize) {
            bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, " \t\r\n");
    }
    tokens[position] = NULL;
    free(line_copy);
    return tokens;
}

int lsh_execute(char **args) {
    if (args[0] == NULL) {
        return 1;
    }

    // Check for built-in commands
    for (int i = 0; i < lsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_srt[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return lsh_launch(args);
}