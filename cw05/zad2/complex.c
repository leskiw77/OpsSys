#include "complex.h"

double absolute(complex c) {
    return sqrt(c.im * c.im + c.re * c.re);
}

complex doubleComplex(complex c) {
    complex n;
    n.re = c.re * c.re - c.im * c.im;
    n.im = 2 * c.re * c.im;
    return n;
}

complex addComplex(complex a, complex b) {
    complex n;
    n.re = a.re + b.re;
    n.im = a.im + b.im;
    return n;
}
