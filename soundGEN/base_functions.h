#ifndef BASE_FUNCTIONS_H
#define BASE_FUNCTIONS_H

#include <math.h>
#include <stdarg.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double round(double t);
#endif

double sqr(double a);
double rect(double t);
double sawtooth(double t);
double tri(double t);
double trans(double t, double t1, double t2, double s1, double s2);
double mmax(int n, ...);
double mmin(int n, ...);
double mix(int n, ...);

#endif // BASE_FUNCTIONS_CPP
