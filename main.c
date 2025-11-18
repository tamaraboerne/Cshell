#include "main.h"

int main(int argc, char *argv[]) {
    // Make stdout unbuffered so Python receives output immediately
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    
    //load config files if any

    //process command line arguments
    lsh_loop();

    //perform any shutdown/cleanup
    
    return EXIT_SUCCESS;
}