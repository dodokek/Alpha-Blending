
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wvla-larger-than="
#pragma GCC diagnostic ignored "-Wstack-usage="

#define SQR(x)  ( (x) * (x) )

double RoundTo (double value, double* exponent = NULL);

int main (int argc, const char** argv)
    {
    int n = argc-1;

    if (!n || n >= 1 && (strcmp (argv[1], "-h") == 0 || strcmp (argv[1], "--help") == 0))
        {
        printf ("Usage: %s [name] [number...]\n", argv[0]);
        return -1;
        }
    
    double data[argc]; // = {};

    const char* name = "";
    strtod (argv[1], (char**) &name);
    if (*name) { name = argv[1]; argv++; n--; }

    if (n <= 0)
        return 0;

    for (int i = 0; i < n; i++)
        {
        data[i] = strtod (argv[i+1], NULL);
        }

    qsort (data, n, sizeof (data[0]), [] (const void* a, const void* b)
        {
        return (int) (*(const double*)a - *(const double*)b);
        });

    double average = 0;
    for (int i = 0; i < n; i++) average += data[i];
    average /= n;

    double stddev = 0;
    for (int i = 0; i < n; i++) stddev += SQR (average - data[i]);
    if (n > 1) stddev = sqrt (stddev / (n-1));

    double median = (n % 2)? data[n/2] : (data [n/2 - 1] + data [n/2]) / 2;

    double exp = NAN;
    stddev  = RoundTo (stddev,  &exp);
    average = RoundTo (average, &exp);

    if (*name) printf ("%s: ", name);
    printf ("{");
    for (int i = 0; i < n; i++) printf ("%s%lg", (i? ", " : ""), data[i]);
    printf ("}\n");

    printf ("Average: %lg +- %lg (%lg%%), Median: %lg, Count: %d\n",
            average, stddev, RoundTo (stddev*100/average), median, n);

    return n;
    }

double RoundTo (double value, double* exponent /*= NULL*/)
    {
    double val = value;
    double exp = pow (10, (int) log10 (fabs (val)));
    double man = val / exp;

    exp = (exponent && !std::isnan (*exponent))? *exponent : ((fabs (man) < 0.3)? 100 : 10) / exp;
    val = round (val * exp) / exp;

    if (exponent) *exponent = exp; 

    return (std::isfinite (val))? val : value;
    }
    