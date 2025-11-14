#include "lsh.h"

void lsh_loop() {
    // Main loop of the shell
    char *line;
    char **args;
    int status;

    do {
        // Print prompt
        printf("> ");
        
        // Read line
        line = lsh_read_line();
        
        // Parse line
        args = lsh_split_line(line);
        
        // Execute command
        status = lsh_execute(args);
        
        // Free memory
        free(line);
        free(args);
        
    } while (status);
}

int lsh_launch(char **args) {
    // Function to launch a program
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
    // Function to read a line of input from the user
    size_t bufsize = LSH_BUFSIZE;
    int position = 0;
    int c;

    char *buffer = malloc(bufsize * sizeof(char));
    if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while(1) {
        // Read a character
        c = getchar();

        // If we hit EOF, replace it with a null character and return
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        // If we have exceeded the buffer, reallocate
        if (position >= bufsize) {
            bufsize += LSH_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    /* ------ in short with getLine() ------

    char *line = NULL;
    ssize_t bufsize = 0; // have getline allocate a buffer for us
    if (getline(&line, &bufsize, stdin) == -1) {
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);  // We received an EOF
        } else  {
            perror("lsh: getline\n");
            exit(EXIT_FAILURE);
        }
    }                
    */

}

char **lsh_split_line(char *line) {
    // Function to split a line into tokens (arguments)
    int bufsize = LSH_TOK_BUFSIZE; 
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, " \t\r\n");
    while (token != NULL) {
        tokens[position] = token;
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
    return tokens;
}

int lsh_execute(char **args) {
    // Function to execute a command
    if (args[0] == NULL) {
        // An empty command was entered
        return 1;
    }

    // Check for built-in commands
    extern int lsh_num_builtins();
    extern char *builtin_srt[];
    extern int (*builtin_func[]) (char **);

    for (int i = 0; i < lsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_srt[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return lsh_launch(args);
}