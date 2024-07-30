#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "options.h"

struct options
parseArgs(int argc, char** argv)
{
    // create struct options and initialize
    struct options options;
    options.input = NULL;
    options.output = NULL;
    
    // iterator
    int op;
    
    // agjust original to include additional args passed
    if (argc >= 2)
    {
        options.nbytes = strtoll(argv[argc-1], NULL, 10);
    }
    else
    {
        options.nbytes = 0;
    }
    
    // check every next option until no more inputs/outputs
    while ((op = getopt(argc, argv, "i:o:")) != -1)
    {
        // if -i flag, set options input to optarg
        if (op == 'i')
        {
            options.input = optarg;
        }
        // if -o flag, set options output to optarg
        else if(op == 'o')
        {
            options.output = optarg;
        }
        // if neither, then exit and print error
        else
        {
            fprintf(stderr, "Error: Incorrect Inputs \n");
            exit(1);
        }
    }
    
    return options;
}
