#ifndef LOCALTEST_COMPLEX_H
#define LOCALTEST_COMPLEX_H

#include <math.h>

typedef struct Complex {
    double re;
    double im;
} complex;

double absolute(complex);

complex doubleComplex(complex);

complex addComplex(complex, complex);

#endif //LOCALTEST_COMPLEX_H

