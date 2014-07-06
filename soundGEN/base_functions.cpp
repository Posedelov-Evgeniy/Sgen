#include <math.h>
#include <stdarg.h>

double sqr(double t)
{
    return t*t;
}

double rect(double t)
{
    return sin(t) >=0 ? 1 : 0;
}

double sawtooth(double t)
{
    t = t / (2 * M_PI);
    return 2*(t-floor(t))-1;
}

double tri(double t)
{
    return 2*fabs(sawtooth(t))-1;
}

double trans(double t, double t1, double t2, double s1, double s2)
{
    if (t<t1) return s1;
    else if(t>t2 || t1==t2) return s2;
    return s1*(t2-t) + s2*(t-t1) / (t2-t1);
}

double mmax(int n, ...)
{
    double val,largest;
    va_list vl;
    va_start(vl,n);
    largest=va_arg(vl,double);
    for (int i=1;i<n;i++)
    {
        val=va_arg(vl,double);
        largest=(largest>val)?largest:val;
    }
    va_end(vl);
    return largest;
}

double mmin(int n, ...)
{
    double val,smallest;
    va_list vl;
    va_start(vl,n);
    smallest=va_arg(vl,double);
    for (int i=1;i<n;i++)
    {
        val=va_arg(vl,double);
        smallest=(smallest<val)?smallest:val;
    }
    va_end(vl);
    return smallest;
}

double mix(int n, ...)
{
    double result = 0;
    va_list vl;
    va_start(vl,n);
    for (int i=0;i<n;i++) result+=va_arg(vl,double);
    va_end(vl);
    return result/n;
}
