#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include "output.h"

bool
writebytes (unsigned long long x, int nbytes)
{
  do
    {
      if (putchar (x) < 0)
    return false;
      x >>= CHAR_BIT;
      nbytes--;
    }
  while (0 < nbytes);

  return true;
}

int
writeoutput(unsigned long long (*rand64) (void), int output, long long nbytes)
{
    int wordsize = sizeof rand64 ();
    int output_errno = 0;
    
    // input passed
    if (output != 0)
    {
       do
       {
           // iterate until nbytes are less than output
           if (nbytes < output)
            output = nbytes;
           
           int size = output/wordsize + 1;
           // allocate malloc for buffer
           unsigned long long* buffer = malloc(size * sizeof(unsigned long long));
           
           if (buffer)
           {
               int itr = 0;
               while (itr < size)
               {
                   // assign each byte in malloc to random number
                   buffer[itr] = rand64();
                   itr++;
               }
               nbytes = nbytes - output;
               // writes the bytes onto file
               write(1,buffer,output);
               // frees allocated malloc
               free(buffer);
           }
           else
           {
               fprintf(stderr, "Error: Buffer Invalid \n");
               exit(1);
           }
       }
        while(nbytes > 0);
    }
    else
    {
        // default stdio output
        do
        {
          unsigned long long x = rand64 ();
          int outbytes = nbytes < wordsize ? nbytes : wordsize;
          if (!writebytes (x, outbytes))
          {
            output_errno = errno;
            break;
          }
          nbytes -= outbytes;
        }
        while (0 < nbytes);
    }
    
    if (fclose (stdout) != 0)
      output_errno = errno;

    if (output_errno)
    {
        errno = output_errno;
        perror ("output");
    }

    return output_errno;
}

