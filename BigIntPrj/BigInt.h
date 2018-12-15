#pragma once
#include <stdio.h>
#include <limits.h>
#include <assert.h>

typedef unsigned int Digit;
typedef unsigned long long DoubleDigit;

//#define BASE10

#ifdef BASE10
#define LAST_DIGIT 9
#define DIGIT_BASE 10ull
#else
#define LAST_DIGIT UINT_MAX
#define DIGIT_BASE 4294967296ull //UINT_MAX + 1
#endif



int Add(Digit* a, Digit aSig, Digit* b, Digit bSig, Digit* result);
int Subtract(Digit* a, Digit aSig, Digit* b, Digit bSig, Digit* result);
int MultiplyByDigit(Digit* a, int aSig, Digit value);
int Multiply(Digit* a, Digit aSig, Digit* b, Digit bSig, Digit* result);
int Parse(const char* psz, Digit* result, int nDigits);

