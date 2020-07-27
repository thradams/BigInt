```c
// bigint2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>

#pragma once
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <string.h>


#define BASE10

#ifdef BASE10
#define LAST_DIGIT 9
#define DIGIT_BASE 10ull
#else
#define LAST_DIGIT UINT_MAX
#define DIGIT_BASE 4294967296ull //UINT_MAX + 1
#endif



int Add(unsigned long a[], unsigned long aSig, unsigned long b[], unsigned long bSig, unsigned long result[]);
int Subtract(unsigned long* a, unsigned long aSig, unsigned long* b, unsigned long bSig, unsigned long* result);
int MultiplyByDigit(unsigned long* a, int aSig, unsigned long value);
int Multiply(unsigned long* a, unsigned long aSig, unsigned long* b, unsigned long bSig, unsigned long* result);
unsigned long Parse(const char* psz, unsigned long result[], int nDigits);

int quotient(unsigned long number[], unsigned long size, unsigned long k);
int to_dec_string(unsigned long number[], int nsize, char out[]);
unsigned long longdiv(unsigned long left[],
             int leftSize,
             unsigned long right[],
             int rightSize,
             unsigned long out[]);

void TestConversion(const char* in, const char* out)
{
    unsigned long number[100] = { 0 };
    unsigned long numberSize = Parse(in, number, 100);
    char str[100];
    to_dec_string(number, numberSize, str);
    assert(strcmp(in, str) == 0);    
}

void TestMultiplication(const char* a, const char* b, const char* result)
{
    unsigned long numberA[100] = { 0 };
    unsigned long numberASize = Parse(a, numberA, 100);

    unsigned long numberB[100] = { 0 };
    unsigned long numberBSize = Parse(b, numberB, 100);
    
    unsigned long numberR[100] = { 0 };
    unsigned long numberRSize = Multiply(numberA, numberASize, numberB, numberBSize, numberR);

    char str[100];
    to_dec_string(numberR, numberRSize, str);
    assert(strcmp(str, result) == 0);

}

void TestDiv(const char* a, const char* b, const char* result)
{
    unsigned long numberA[100] = { 0 };
    unsigned long numberASize = Parse(a, numberA, 100);

    unsigned long numberB[100] = { 0 };
    unsigned long numberBSize = Parse(b, numberB, 100);

    unsigned long numberR[100] = { 0 };
    unsigned long numberRSize = longdiv(numberA, numberASize, numberB, numberBSize, numberR);

    char str[100];
    to_dec_string(numberR, numberRSize, str);
    assert(strcmp(str, result) == 0);

}


void TestAdd(const char* a, const char* b, const char* result)
{
    unsigned long numberA[100] = { 0 };
    unsigned long numberASize = Parse(a, numberA, 100);

    unsigned long numberB[100] = { 0 };
    unsigned long numberBSize = Parse(b, numberB, 100);

    unsigned long numberR[100] = { 0 };
    unsigned long numberRSize = Add(numberA, numberASize, numberB, numberBSize, numberR);

    char str[100];
    to_dec_string(numberR, numberRSize, str);
    assert(strcmp(str, result) == 0);

}


void TestSubtration(const char* a, const char* b, const char* result)
{
    unsigned long numberA[100] = { 0 };
    unsigned long numberASize = Parse(a, numberA, 100);

    unsigned long numberB[100] = { 0 };
    unsigned long numberBSize = Parse(b, numberB, 100);

    unsigned long numberR[100] = { 0 };
    unsigned long numberRSize = Subtract(numberA, numberASize, numberB, numberBSize, numberR);

    char str[100];
    to_dec_string(numberR, numberRSize, str);
    assert(strcmp(str, result) == 0);

}

int main()
{
    TestConversion("0", "0");
    TestConversion("1", "1");
    TestConversion("4294967295", "4294967295"); //UINT_MAX
    TestConversion("4294967296", "4294967296"); //UINT_MAX + 1
    TestConversion("18446744073709551616", "18446744073709551616"); //ULL_MAX 
    TestConversion("18446744073709551617", "18446744073709551617"); //ULL_MAX  +1 
    
    TestAdd("0", "0", "0");
    TestAdd("0", "1", "1");
    TestAdd("1", "0", "1");
    TestAdd("1", "1", "2");
    TestAdd("4294967295", "1", "4294967296" );

    //2000
    //1999

    //unsigned long numberR[100] = { 4294967295, 4294967295, 4294967295, 1 };
    
//    char str[100];
    //to_dec_string(numberR, 4, str);

    TestSubtration("158456325028528675187087900672", "158456325028528675187087900671", "1");

    TestMultiplication("1", "1", "1");
    TestMultiplication("4294967295", "4294967295", "18446744065119617025");
    TestMultiplication("18446744065119617025", "18446744065119617025", "340282366604025813516997721482669850625");
    
    TestMultiplication("0", "0", "0");
    TestMultiplication("1", "0", "0");
    TestMultiplication("0", "1", "0");
    
    TestDiv("489", "12", "40");
    TestDiv("100", "10", "10");
    TestDiv("340282366604025813516997721482669850625", "4294967295", "79228162458924105385300197375");
    

    
}

/**************************************/
/* 4655                               */
/*   45  ******************************/
/*-----                               */
/*                                    */
/**************************************/

int Add(unsigned long a[], unsigned long aSig,
        unsigned long b[], unsigned long bSig,
        unsigned long result[])
{
    unsigned long long carry = 0;
    int i = 0;

    int maxSig = aSig > bSig ? aSig : bSig;

    for (; i < maxSig; i++)
    {
        unsigned long long aj = i < aSig ? a[i] : 0;
        unsigned long long bj = i < bSig ? b[i] : 0;
        unsigned long long current = aj + bj + carry;
        result[i] = current % DIGIT_BASE;
        carry = current / DIGIT_BASE;
    }

    if (carry != 0)
    {
        result[i] = carry;
        i++;
    }

    //Retona o número de digitos escritos
    return i;
}


int Subtract(unsigned long* a, unsigned long aSig, unsigned long* b, unsigned long bSig, unsigned long* result)
{
    assert(aSig >= bSig);

    int nDigits = 0;
    unsigned long long borrow = 0;

    long long diff = 0;//must be signed

    for (; nDigits < aSig; nDigits++)
    {
        unsigned long long aj = nDigits < aSig ? a[nDigits] : 0;
        unsigned long long bj = nDigits < bSig ? b[nDigits] : 0;
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

int MultiplyByDigit(unsigned long* a, int aSig, unsigned long value)
{
    unsigned long long carry = 0;
    int i = 0;
    for (; i < aSig; i++)
    {
        unsigned long long temp = a[i];
        temp *= value;
        temp += carry;
        a[i] = temp % DIGIT_BASE;
        carry = temp / DIGIT_BASE;
    };

    if (carry != 0)
    {
        a[i] = carry;
        i++;
    }

    return i;
};

int Multiply(unsigned long* a, unsigned long aSig,
             unsigned long* b, unsigned long bSig,
             unsigned long* result)
{
    /*
       22
      1234 a
        56 b
      ----
      7404
     6170
     ------
     69104

    */
    for (size_t j = 0; j < bSig; ++j)
    {
        if (b[j] == 0)
        {
            continue;
        }

        unsigned long long k = 0;

        for (size_t i = 0; i < aSig; ++i)
        {
            unsigned long long ui = a[i];
            unsigned long long vj = b[j];
            unsigned long long rij = j > 0 ? result[i + j] : 0;
            unsigned long long current = ui * vj + rij + k;
            result[i + j] = current % DIGIT_BASE;
            k = current / DIGIT_BASE;
        }

        result[j + aSig] = k;
    }

    if (result[aSig + bSig-1] == 0)
    {
        return aSig + bSig - 1;
    }
    else
    {
        return aSig + bSig;
    }

    
}

//Retorna o número de digitos significativos
//Exemplo: 000456 retorna 3
int sig_digits(unsigned long number[], int size)
{
    if (size == 0)
        return 0;

    for (int k = size - 1; k >= 0; k--)
    {
        if (number[k] != 0)
        {
            return k + 1;
        }
    }

    return size;
}

/*
   Retorna o modulo da divisao do number por um unico digito   
*/
unsigned long long mod(unsigned long number[], int size, unsigned long value)
{
    unsigned long long carry = 0;
    for (int i = size - 1; i >= 0; i--)
    {
        carry = (carry * DIGIT_BASE + number[i]) % value;
    }
    return carry;
}

void subtract_in_place(unsigned long left[],
                       int leftSize,
                       unsigned long right[],
                       int rightSize)
{

    unsigned long long borrow = 0;

    for (int i = 0; i < leftSize; i++)
    {
        long long  ri_k = left[i];
        long long  dqi = i < rightSize ? right[i] : 0;
        long long  diff = ri_k - dqi;
        diff = diff - borrow + DIGIT_BASE;
        left[i] = diff % DIGIT_BASE;
        borrow = 1 - diff / DIGIT_BASE;
    }

    assert(borrow == 0);
}

unsigned long multiply_in_place(unsigned long left[],
                       int leftSize,
                       unsigned long value)
{
    unsigned long long carry = 0;
    int i = 0;
    for (; i < leftSize; i++)
    {
        unsigned long long temp = left[i];
        temp *= value;
        temp += carry;
        left[i] = temp % DIGIT_BASE;
        carry = temp / DIGIT_BASE;
    };
    if (carry != 0)
    {
        left[i] = carry;
        i++;
    }
    return i;
};


int compare(unsigned long  left[],
            int leftSize,
            unsigned long right[],
            int rightSize)
{

    int i = leftSize > rightSize ? leftSize : rightSize;

    for (; i != 0; i--)
    {
        unsigned long left_comp = (i < leftSize) ? left[i] : 0;
        unsigned long right_comp = (i < rightSize) ? right[i] : 0;

        if (left_comp < right_comp)
        {
            return -1;
        }
        else if (left_comp > right_comp)
        {
            return  1;
        }
    }

    return 0;
}


int quotient(unsigned long number[], unsigned long size, unsigned long k)
{
    unsigned long m = sig_digits(number, size);
    unsigned long long carry = 0;
    
    for (int i = m - 1; i >= 0; i--)
    {
        unsigned long long temp = carry * DIGIT_BASE + number[i];
        number[i] = temp / k;
        carry = temp % k;
    }
    return m;
}


unsigned long trialdigit(unsigned long r[], unsigned long rsize,
                         unsigned long d[], unsigned long dsize,
                         unsigned long k,
                         unsigned long m)
{
    assert(m >= 2);
    assert(k + m >= m);
    

    unsigned long km = k + m;
    unsigned long r3 = (r[km] * DIGIT_BASE + r[km - 1]) * DIGIT_BASE + r[km - 2];
    unsigned long d2 = d[m - 1] * DIGIT_BASE + d[m - 2];

    unsigned long c1 = r3 / d2;
    unsigned long c2 = DIGIT_BASE - 1;
    return c1 < c2 ? c1 : c2;
}

void longmod(unsigned long left[],
             int leftSize,
             unsigned long right[],
             int rightSize,
             unsigned long out[])
{


    const size_t n = sig_digits(left, leftSize);
    const size_t m = sig_digits(right, rightSize);


    //unsigned long long sigvlast = right[sig_digits(right, rightSize)-1];
    unsigned long long ym_1 = right[sig_digits(right, rightSize) - 1];
    unsigned long long f = DIGIT_BASE / (ym_1 + 1);
    multiply_in_place(left, leftSize, f);
    multiply_in_place(right, rightSize, f);

    for (int k = n - m; k >= 0; k--)
    {
        unsigned long long qt = trialdigit(left, leftSize, right, rightSize, (size_t)k, (size_t)m);

        //std::vector<unsigned int> dq((right_last - right_first) + 1);
        unsigned long dq[1000];

        MultiplyByDigit(right, rightSize, qt);

        if (compare(&left[k], leftSize -(k + m), dq, m) == -1)
        {
            qt = qt - 1;
            MultiplyByDigit(left, leftSize, qt);
        }

        subtract_in_place(&left[k], leftSize-(k + m), dq, 1000- m);

        //cleaning non used digits
        for (int i = k + m; i < n; i++)
        {
            left[i] = 0;
        }

        //println2(left_first, left_last);
    }

    
    quotient(left, leftSize, f);
};

 
unsigned long longdiv(unsigned long left[],
             int leftSize,
             unsigned long right[],
             int rightSize,
             unsigned long out[])
{


    assert(sig_digits(left, leftSize) == leftSize);
    assert(sig_digits(right, rightSize) == rightSize);

    const size_t n = sig_digits(left, leftSize);
    const size_t m = sig_digits(right, rightSize);
    assert(m >= 2);
    assert(n >= m);
    unsigned long long ym_1 = right[rightSize-1];
    unsigned long long f = DIGIT_BASE / (ym_1 + 1);
    leftSize = multiply_in_place(left, leftSize, f);
    rightSize = multiply_in_place(right, rightSize, f);

    for (int k = n - m; k >= 0; k--)
    {
        
        assert(m <= k + m);
        assert(k + m <= n);
        unsigned long  qt = trialdigit(left, leftSize, right, rightSize, k, m);


        unsigned long dq[1000] = {0};

        unsigned int dqsize = Multiply(right, rightSize, &qt, 1, dq);

        if (compare(&left[k], m/*leftSize - (k + m)*/, dq, dqsize - m) == -1)
        {
            qt = qt - 1;
            MultiplyByDigit(left, leftSize, qt);
        }

        out[k] = qt;
        subtract_in_place(&left[k], m, dq, m);

        //cleaning non used digits
        for (int i = k + m; i < n; i++)
        {
            left[i] = 0;
        }

        //println2(left_first, left_last);
    }
    return sig_digits(left, leftSize);
};




bool is_zero(unsigned long number[], int nsize)
{
    for (int i =0; i < nsize; i++)
        if (number[i] != 0)
            return false;
    return true;
}



int to_dec_string(unsigned long number[], int nsize, char out[])
{
    
    if (nsize == 1 && number[0] == 0)
    {
        out[0] = '0';
        out[1] = '\0';
        return 1;
    }

    int i = 0;
    while (!is_zero(number, nsize))
    {
        char ch = char(mod(number, nsize, 10) + '0');
        out[i] = ch;
        i++;
        nsize = quotient(number, nsize, 10);
    }
    out[i] = '\0';

    for (int j = 0; j < i / 2; j++)
    {
        char temp = out[j];
        out[j] = out[i - j - 1];
        out[i - j - 1] = temp;        
    }

    
    return i;
}

/*
  Converte a string psz em result[].
  Retorna o numero de digitos escritos
*/
unsigned long Parse(const char* psz, unsigned long result[], int nDigits)
{

    //clear
    result[0] = 0;
    int resultSize = 1;

    unsigned long decimalDigit = *psz - '0';
    resultSize = Add(result, resultSize, &decimalDigit, 1, result);
    psz++;

    int i = 0;
    while ((*psz >= '0') && (*psz <= '9'))
    {
        resultSize = multiply_in_place(result, resultSize, 10);
        decimalDigit = *psz - '0';
        resultSize = Add(result, resultSize, &decimalDigit, 1, result);
        psz++;
    }

    return resultSize;
}

```

