#ifndef BASE_FUNCTIONS_CPP
#define BASE_FUNCTIONS_CPP

#include <math.h>
#include <stdarg.h>
#include "base_functions.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
    // vs2012 hack
    #ifndef FP_INFINITE
        double round(double t)
        {
            return floor(t + 0.5);
        }
    #endif
#endif

/*
    {{sin(t)}}
    Категория: сигналы.
    Возвращает синус аргумента t.
    При генерации сигнала заданной частоты должен использоваться как sin(k*t)
*/

/*
    {{cos(t)}}
    Категория: сигналы.
    Возвращает косинус аргумента t.
    При генерации сигнала заданной частоты должен использоваться как cos(k*t)
*/

/*
    {{tan(t)}}
    Категория: сигналы.
    Возвращает тангенс аргумента t.
    При генерации сигнала заданной частоты должен использоваться как tan(k*t)
*/

/*
    {{sqr(a)}}
    Категория: другие функции.
    Возвращает квадрат аргумента a.
*/
double sqr(double a)
{
    return a*a;
}

/*
    {{rect(t)}}
    Категория: сигналы.
    Возвращает значение периодического прямоугольного сигнала соответствующего t.
    При генерации сигнала заданной частоты должен использоваться как rect(k*t)
*/
double rect(double t)
{
    return sin(t) >=0 ? 1 : 0;
}

/*
    {{sawtooth(t)}}
    Категория: сигналы.
    Возвращает значение пилообразного сигнала соответствующего t.
    При генерации сигнала заданной частоты должен использоваться как sawtooth(k*t)
*/
double sawtooth(double t)
{
    t = t / (2 * M_PI);
    return 2*(t-floor(t))-1;
}

/*
    {{tri(t)}}
    Категория: сигналы.
    Возвращает значение треугольного сигнала соответствующего t.
    При генерации сигнала заданной частоты должен использоваться как tri(k*t)
*/
double tri(double t)
{
    return 2*fabs(sawtooth(t))-1;
}

/*
    {{trans(t,t1,t2,s1,s2)}}
    Категория: объединение сигналов.
    Возвращает переходный сигнал между s1 и s2 для времени t.
    Переход - линейный. Выполняется при t между t1 и t2.
    При t<t1 возвращается сигнал s1.
    При t>t2 возвращается сигнал s2.
    t,t1,t2 измеряются в секундах.
*/
double trans(double t, double t1, double t2, double s1, double s2)
{
    if (t<t1) return s1;
    else if(t>t2 || t1==t2) return s2;
    return s1*(t2-t) + s2*(t-t1) / (t2-t1);
}

/*
    {{mmax(n,s1,s2,...)}}
    Категория: объединение сигналов.
    Возвращает максимальный из сигналов s1,s2...sn.
    n - количество сигналов.
*/
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

/*
    {{mmin(n,s1,s2,...)}}
    Категория: объединение сигналов.
    Возвращает минимальный из сигналов s1,s2...sn.
    n - количество сигналов.
*/
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

/*
    {{mix(n,s1,s2,...)}}
    Категория: объединение сигналов.
    Возвращает смешанный сигнал из сигналов s1,s2...sn.
    n - количество сигналов.
*/
double mix(int n, ...)
{
    double result = 0;
    va_list vl;
    va_start(vl,n);
    for (int i=0;i<n;i++) result+=va_arg(vl,double);
    va_end(vl);
    return result/n;
}

base_function_def newDef(base_function_signal sifn, std::string sinm) {
    base_function_def rdef;
    rdef.name = sinm;
    rdef.function = sifn;
    return rdef;
}

void getBaseFunctions(std::vector<base_function_def>* base_f) {
    base_f->push_back(newDef(sin,"sin"));
    base_f->push_back(newDef(cos,"cos"));
    base_f->push_back(newDef(tan,"tan"));
    base_f->push_back(newDef(rect,"rect"));
    base_f->push_back(newDef(sawtooth,"sawtooth"));
    base_f->push_back(newDef(tri,"tri"));
    return;
}

#endif // BASE_FUNCTIONS_CPP
