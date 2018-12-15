#include "BigInt.h"



int Add(Digit* a, Digit aSig, Digit* b, Digit bSig, Digit* result)
{
    DoubleDigit carry = 0;
    int nDigits = 0;

    int maxSig = aSig > bSig ? aSig : bSig;

    for (; nDigits < maxSig; nDigits++)
    {
        DoubleDigit aj = nDigits < aSig ? a[nDigits] : 0;
        DoubleDigit bj = nDigits < bSig ? b[nDigits] : 0;
        DoubleDigit current = aj + bj + carry;
        result[nDigits] = current % DIGIT_BASE;
        carry = current / DIGIT_BASE;
    }

    if (carry != 0)
    {
        result[nDigits] = carry;
        nDigits++;
    }

    return nDigits;
}


int Subtract(Digit* a, Digit aSig, Digit* b, Digit bSig, Digit* result)
{
    assert(aSig >= bSig);

    int nDigits = 0;
    DoubleDigit borrow = 0;

    long long diff = 0;//must be signed

    for (; nDigits < aSig; nDigits++)
    {
        DoubleDigit aj = nDigits < aSig ? a[nDigits] : 0;
        DoubleDigit bj = nDigits < bSig ? b[nDigits] : 0;
        diff = aj - bj;

        diff = diff - borrow + DIGIT_BASE;
        result[nDigits] = diff % DIGIT_BASE;
        borrow = 1 - diff / DIGIT_BASE;
    }

    //result is the number of signiicative digits
    while (nDigits > 1 && result[nDigits - 1] == 0)
        nDigits--;

    return nDigits;
}

int MultiplyByDigit(Digit* a, int aSig, Digit value)
{
    DoubleDigit carry = 0;
    int nDigits = 0;
    for (; nDigits < aSig; nDigits++)
    {
        DoubleDigit temp = a[nDigits];
        temp *= value;
        temp += carry;
        a[nDigits] = temp % DIGIT_BASE;
        carry = temp / DIGIT_BASE;
    };

    if (carry != 0)
    {
        a[nDigits] = carry;
        nDigits++;
    }

    return nDigits;
};

int Multiply(Digit* a, Digit aSig, Digit* b, Digit bSig, Digit* result)
{
    for (size_t j = 0; j < bSig; ++j)
    {
        if (b[j] == 0)
        {
            continue;
        }

        DoubleDigit k = 0;

        for (size_t i = 0; i < aSig; ++i)
        {
            DoubleDigit ui = a[i];
            DoubleDigit vj = b[j];
            DoubleDigit rij = j > 0 ? result[i + j] : 0;
            DoubleDigit current = ui * vj + rij + k;
            result[i + j] = current % DIGIT_BASE;
            k = current / DIGIT_BASE;
        }

        result[j + aSig] = k;
    }
    return 0;
}


int Parse(const char* psz, Digit* result, int nDigits)
{
    int localdigits = 0;
    if ((*psz >= '0') && (*psz <= '9'))
    {
        Digit number = *psz - '0';
        localdigits = Add(result, 1, &number, 1, result);
        psz++;

        while ((*psz >= '0') && (*psz <= '9'))
        {
            localdigits = MultiplyByDigit(result, localdigits, 10);
            number = *psz - '0';
            localdigits = Add(result, localdigits, &number, 1, result);
            psz++;
        }
    }
    return localdigits;
}