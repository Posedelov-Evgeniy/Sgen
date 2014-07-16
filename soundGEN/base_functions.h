#ifndef BASE_FUNCTIONS_H
#define BASE_FUNCTIONS_H

#include <stdlib.h>
#include <vector>
#include <string>
#include <algorithm>
#include <math.h>
#include <stdarg.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
    #ifndef M_PI
        #define M_PI 3.14159265358979323846
    #endif
    double round(double t);
#endif

typedef double (*base_function_signal) (double);

struct base_function_def {
    std::string name;
    base_function_signal function;
};

double sqr(double a);
double rect(double t);
double sawtooth(double t);
double tri(double t);
double trans(double t, double t1, double t2, double s1, double s2);
double mmax(int n, ...);
double mmin(int n, ...);
double mix(int n, ...);

void getBaseFunctions(std::vector<base_function_def>* base_f);

#endif // BASE_FUNCTIONS_H
