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


int Compare(Digit* a, Digit aSig, Digit* b, Digit bSig)
{
    size_t i = aSig > bSig ? aSig : bSig;
    for (;;)
    {
        Digit ai = (i < a) ? a[i] : 0;
        Digit bi = (i < b) ? b[i] : 0;

        if (ai < bi)
        {
            return -1;
        }
        else if (ai > bi)
        {
            return  1;
        }

        if (i == 0)
        {
            break;
        }

        i--;
    }

    return 0;
}
Digit TrialDigit(Digit* r,
    Digit* d,
    size_t k,
    size_t m)
{
    // r{3} = r[k + m .. k + m - 2]
    // d{2} = d[m - 1..m-2]
    size_t km = k + m;
    double rkm = r[km];
    double rkm_1 = r[km - 1];
    double rkm_2 = r[km - 2];
    double dm_1 = d[m - 1];
    double dm_2 = d[m - 2];
    double r3 = (rkm * DIGIT_BASE + rkm_1) * DIGIT_BASE + rkm_2;
    double d2 = dm_1 * DIGIT_BASE + dm_2;
    double d4 = r3 / d2;
    return d4 < (double)(DIGIT_BASE - 1) ? d4 : (double)(DIGIT_BASE - 1);
}

int Divide(Digit* a, Digit aSig, Digit* b, Digit bSig, Digit* result)
{
    int n = aSig;
    int m = bSig;
    Digit  localA[10] = { 0 };
    Digit  localB[10] = { 0 };
    Digit  localC[10] = { 0 };

    Digit ym_1 = b[bSig];//rev
    Digit f = (Digit)  (DIGIT_BASE / (ym_1 + 1));
    
    Multiply(a, aSig, &f, 1, localA);
    Multiply(b, aSig, &f, 1, localB);


    for (int k = n - m; k >= 0; k--)
    {
        Digit qt = TrialDigit(localA, localB, (size_t)k, (size_t)m);

        Multiply(localB, bSig, &qt, 1, localC);

        if (Compare(localA + k, k + m, localC,  m) == -1)
        {
            qt = qt - 1;
            Multiply(localB, bSig, &qt, 1, localC);
        }

        result[k] = qt;
        Subtract(localA + k, m, localC,  m, localA);
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