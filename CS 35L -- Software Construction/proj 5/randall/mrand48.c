#include <stdlib.h>
#include "mrand48.h"

struct drand48_data buffer = {};

void mrand48_r_init (void)
{
}

unsigned long long use_mrand48_r (void)
{
    long int result;
    mrand48_r(&buffer, &result);
    return (unsigned long long) result;
}

void mrand48_r_fini (void)
{
}

