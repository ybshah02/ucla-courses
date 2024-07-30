#ifndef OUTPUT_H
#define OUTPUT_H

bool writebytes (unsigned long long x, int nbytes);
int writeoutput(unsigned long long (*rand64) (void), int output, long long nbytes);

#endif
