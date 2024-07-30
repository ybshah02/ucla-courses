#ifndef CNN_CUH
#define CNN_CUH

#include <string>

#define kNum 256
#define kInImSize 228
#define kImSize 224
#define kOutImSize 112
#define kKernel 5

#define input(i,h,w)    input[(i)*kInImSize*kInImSize + (h)*kInImSize + (w)]
#define output(i,h,w)   output[(i)*kOutImSize*kOutImSize + (h)*kOutImSize + (w)]
#define weight(i,j,p,q) weight[(i)*kNum*kKernel*kKernel + (j)*kKernel*kKernel + (p)*kKernel + (q)]

#endif