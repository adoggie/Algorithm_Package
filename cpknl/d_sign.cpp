#include "f2c.h"

#ifdef KR_headers
extern "C" double d_sign(a,b) doublereal *a, *b;
#else
extern "C" double d_sign(doublereal *a, doublereal *b)
#endif
{
double x;
x = (*a >= 0 ? *a : - *a);
return( *b >= 0 ? x : -x);
}
