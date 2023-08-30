#ifndef OPTIONS_H
#define OPTIONS_H

struct options
{
    int nbytes;
    char* input;
    char* output;
};

struct options parseArgs(int argc, char** argv);

#endif
