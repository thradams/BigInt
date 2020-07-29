```c

#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include < time.h>

//#define BASE10

#ifdef BASE10
#define LAST_DIGIT 9
#define DIGIT_BASE 10ull
#else
#define LAST_DIGIT UINT_MAX
#define DIGIT_BASE 4294967296ull //UINT_MAX + 1
#endif

int sig_digits(unsigned long number[], int size);
bool is_zero(unsigned long number[], int nsize);
int compare(unsigned long  left[],
            int leftSize,
            unsigned long right[],
            int rightSize);

/*result tem que ser maior entre aSig e bSig + 1*/
int Add(unsigned long a[], unsigned long aSig, unsigned long b[], unsigned long bSig, unsigned long result[]);
/*result tem que ser igual ao maior*/
int Subtract(unsigned long* a, unsigned long aSig, unsigned long* b, unsigned long bSig, unsigned long* result);


/*result tem que ter o tamanho de aSig + 1*/
int MultiplyByDigit(unsigned long* a, int aSig, unsigned long value);

/*result tem que ter o tamanho de aSig + bSig*/
int Multiply(const unsigned long a[], unsigned long aSig,
             const unsigned long b[], unsigned long bSig,
             unsigned long result[]);

unsigned long Parse(const char* psz, unsigned long result[], int nDigits);

/*result tem que ter o tamanho do right*/
unsigned long modulo(unsigned long left[],
                     int leftSize,
                     unsigned long right[],
                     int rightSize,
                     unsigned long out[]);
int quotient2(unsigned long number[], unsigned long size, unsigned long k,
              unsigned long out[]);
int quotient(unsigned long number[], unsigned long size, unsigned long k);
int to_dec_string(unsigned long number[], int nsize, char out[]);
char* to_string(unsigned long number[], int nsize, char out[]);
char* to_string_repr(unsigned long number[], int nsize, char out[]);

unsigned int modular_pow(unsigned long base[],
                         unsigned long baseSize,
                         unsigned long exponent[],
                         unsigned long exponentSize,
                         unsigned long modulus[],
                         unsigned long modulusSize,
                         unsigned long result[]);

/*result tem que ter o tamanho do left*/
unsigned long longdiv(unsigned long left[],
                      int leftSize,
                      unsigned long right[],
                      int rightSize,
                      unsigned long out[]);

/*result tem que ter o tamanho do left*/
unsigned long division(unsigned long left[],
                       int leftSize,
                       unsigned long right[],
                       int rightSize,
                       unsigned long out[]);

bool is_prime(unsigned long number[], int ndigits)
{
    /*
    https://en.wikipedia.org/wiki/Primality_test
function is_prime(n)
    if n ≤ 3 then
        return n > 1
    else if n mod 2 = 0 or n mod 3 = 0
        return false

    let i ← 5

    while i × i ≤ n do
        if n mod i = 0 or n mod (i + 2) = 0
            return false
        i ← i + 6

    return true


*/

/*
function isPrime(p) {
for (let i = 2n; i * i <= p; i++) {
  if (p % i === 0n) return false;
}
return true
}
    */

    unsigned long _3[] = { 3 };
    unsigned long _2[] = { 2 };
    unsigned long temp[100] = { 0 };
    unsigned long tempsize = 0;

    //n ≤ 3
    if (compare(number, ndigits, _3, 1) <= 0)
    {
        unsigned long _1[] = { 1 };
        return compare(number, ndigits, _1, 1) > 0;
    }
    else
    {
        tempsize = modulo(number, ndigits, _2, 1, temp);

        //n mod 2 = 0
        if (is_zero(temp, tempsize))
            return false;

        //n mod 3 = 0
        tempsize = modulo(number, ndigits, _3, 1, temp);

        if (is_zero(temp, tempsize))
            return false;
    }

    unsigned long i[100] = { 0 };
    i[0] = 5;
    unsigned long isize = 1;

    unsigned long ii[100] = { 0 };
    unsigned long iisize = Multiply(i, isize, i, isize, ii);
    while (compare(ii, iisize, number, ndigits) <= 0)
    {
        unsigned long t2[100] = { 0 };
        unsigned long t2size = modulo(number, ndigits, i, isize, t2);
        if (is_zero(t2, t2size))
            return false;

        unsigned long i_plus_2[100] = { 0 };
        unsigned long i_plus_2_size = Add(i, isize, _2, 1, i_plus_2);

        t2size = modulo(number, ndigits, i_plus_2, i_plus_2_size, t2);
        if (is_zero(t2, t2size))
            return false;


        unsigned long _6[] = { 6 };
        isize = Add(i, isize, _6, 1, i); /*atualiza i */
        iisize = Multiply(i, isize, i, isize, ii); /*atualiza i * i */
    }

    return true;
}



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

unsigned int modular_pow0(unsigned long base,
                          unsigned long exponent,
                          unsigned long modulus)
{

    if (modulus == 1)
        return 0;

    //Assert :: (modulus - 1) * (modulus - 1) does not overflow base
    unsigned long result = 1;
    base = base % modulus;
    while (exponent > 0) {
        if (exponent % 2 == 1)
        {
            result = (result * base) % modulus;
        }
        exponent = exponent >> 1;
        base = (base * base) % modulus;

    }
    return result;
}

void TestExpMod(const char* a, const char* e, const char* m, const char* result)
{
    //15955 / 7996

    unsigned long numberA[100] = { 0 };
    unsigned long numberASize = Parse(a, numberA, 100);

    unsigned long numberB[100] = { 0 };
    unsigned long numberBSize = Parse(e, numberB, 100);

    unsigned long numberC[100] = { 0 };
    unsigned long numberCSize = Parse(m, numberC, 100);

    unsigned long numberR[100] = { 0 };
    unsigned long numberRSize = modular_pow(numberA, numberASize, numberB, numberBSize, numberC, numberCSize, numberR);

    char str[100];
    to_dec_string(numberR, numberRSize, str);
    assert(strcmp(str, result) == 0);


}



void TestDiv(const char* a, const char* b, const char* result, const char* mod)
{
    //15955 / 7996


    unsigned long numberA[100] = { 0 };
    unsigned long numberASize = Parse(a, numberA, 100);

    unsigned long numberB[100] = { 0 };
    unsigned long numberBSize = Parse(b, numberB, 100);

    unsigned long numberR[100] = { 0 };
    unsigned long numberRSize = division(numberA, numberASize, numberB, numberBSize, numberR);

    char str[100];
    to_dec_string(numberR, numberRSize, str);
    assert(strcmp(str, result) == 0);

    numberR[0] = { 0 };
    numberRSize = modulo(numberA, numberASize, numberB, numberBSize, numberR);


    to_dec_string(numberR, numberRSize, str);
    assert(strcmp(str, mod) == 0);

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

unsigned long copy_to(unsigned long a[], unsigned long asize, unsigned long out[])
{
    for (int i = 0; i < asize; i++)
    {
        out[i] = a[i];
    }
    return asize;
}

/*
GCD (Greatest Common Divisor) or HCF (Highest Common Factor) of two numbers is the largest number that divides both of them.
For example GCD of 20 and 28 is 4 and GCD of 98 and 56 is 14.
*/
unsigned long gcd(unsigned long a[], unsigned long asize, unsigned long b0[], unsigned long bsize, unsigned long out[])
{
    unsigned long b[100];
    copy_to(b0, bsize, b);

    copy_to(a, asize, out);

    /*
    function gcd(a, b)
        while a ≠ b
            if a > b
                a := a − b
            else
                b := b − a
        return a

        function gcd(a, b)
        while b ≠ 0
            t := b
            b := a mod b
            a := t
        return a
    */
    while (!is_zero(b, bsize))
    {
        unsigned long t[100];
        unsigned long tsize = copy_to(b, bsize, t);
        bsize = modulo(out, asize, b, bsize, b);

        asize = copy_to(t, tsize, out);
    }
    return sig_digits(out, asize);
}

/*
 LCM (Least Common Multiple) of two numbers is the smallest number which can be divided by both numbers.
 For example LCM of 15 and 20 is 60 and LCM of 5 and 7 is 35.
*/

unsigned long lcm(unsigned long a[], unsigned long asize, unsigned long b[], unsigned long bsize, unsigned long out[])
{
    assert(asize + bsize < 100);
    unsigned long r1[100];
    unsigned long r1size = Multiply(a, asize, b, bsize, r1);

    unsigned long r2[100];
    unsigned long r2size = gcd(a, asize, b, bsize, r2);
    return division(r1, r1size, r2, r2size, out);
}


void RSATest()
{
    //Steps of RSA implementation :

    //1. Select two prime numbers, p = 3 and q = 11
    //  2. Calculate n = pq = 3 x 11 = 33
    //    3. Calculate m = (3 - 1) (11 - 1) = 2 x 10 = 20
    //    4. Select e such that e is relatively prime to m = 20 and less than m, 1 < e < 20

    //    gcd(e, 20) = 1

    //    20 = 1 x 2 x 10

    //    = 1 x 2 x 2 x 5

    //    = 1 x 22 x 5

    //    Assume that we take the smallest prime number in between of 2, 5 to 20, so e = 3

    //    You also can take other value as long as it fulfils the conditions – it must be relatively prime with 20 and it is less than 20

    //    .Determine d such that de = 1 mod 20 and d < 20

    //    de mod m = 1

    //    d x 3 = (? X 20) + 1

  //      Find the value which when the results mod 3 with no remainder

  //      The correct value is d = 7, because

   //     7 x 3 = 21 = 1 x 20 + 1

    //    You also can take other value as long as it is a positive integer

    //   Therefore, n = 33, m = 20, e = 3, d = 7

   //     The resulting keys are :

      // Public key = { e, n } and Private key = { d, n }
     //   Public key = { 3, 33 } and Private key = { 7, 33 }

       // Assume that P is the plaintext and C is the ciphertext

        //The encryption is C = P3 mod 33
        //The decryption is P = C7 mod 33

        //Public key = { e, n } and Private key = { d, n }
        //Public key = { 3, 33 } and Private key = { 7, 33 }

        //Given P = 6
        //The encryption is C = P3 mod 33
        //C = 63 mod 33
        //= 216 mod 33
        //= 18

        //The decryption is P = C7 mod 33
        //P = 187 mod 33
        //= 6
}
int main()
{
    TestExpMod("2790", "413", "3233", "65");

    TestMultiplication("65", "1", "65");
    TestMultiplication("1", "65", "65");

    modular_pow0(65, 17, 3233);
    TestExpMod("65", "17", "3233", "2790");



    TestExpMod("123", "3", "4", "3");
    TestExpMod("312312312324324324312312312", "234324324324324324324", "5", "1");



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
    TestAdd("4294967295", "1", "4294967296");

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


    TestDiv("489", "12", "40", "9");
    TestDiv("20142", "1013", "19", "895");
    TestDiv("100", "10", "10", "0");
    TestDiv("15955", "7996", "1", "7959");
    TestDiv("26027", "13091", "1", "12936");


    TestDiv("316097", "102", "3098", "101");
    TestDiv("489", "1", "489", "0");
    TestDiv("210", "3", "70", "0");


    TestDiv("340282366604025813516997721482669850625", "4294967295", "79228162458924105385300197375", "0");

    /* Intializes random number generator */
    time_t t;
    srand((unsigned)time(&t));

    for (int i = 0; i < 9000000; i++)
    {

        int a = rand();
        int b = rand();
        if (a > b && b != 0)
        {
            char s1[100];
            _itoa_s(a, s1, 100, 10);

            char s2[100];
            _itoa_s(b, s2, 100, 10);

            char r[100];
            _itoa_s(a / b, r, 100, 10);

            char r2[100];

            _itoa_s(a % b, r2, 100, 10);
            TestDiv(s1, s2, r, r2);

            //
            _itoa_s(a * b, r2, 100, 10);
            TestMultiplication(s1, s2, r2);

            _itoa_s(a + b, r2, 100, 10);
            TestAdd(s1, s2, r2);

            _itoa_s(a - b, r2, 100, 10);
            TestSubtration(s1, s2, r2);

            //
            //_itoa_s(a % b, r2, 100, 10);

            //TestModule(s1, s2, r2);

        }
    }

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
        assert(carry == 0 || carry == 1);
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

int MultiplyByDigit2(unsigned long* a, int aSig, unsigned long value, unsigned long out[])
{
    unsigned long long carry = 0;
    int i = 0;
    for (; i < aSig; i++)
    {
        unsigned long long temp = a[i];
        temp *= value;
        temp += carry;
        out[i] = temp % DIGIT_BASE;
        carry = temp / DIGIT_BASE;
    };

    if (carry != 0)
    {
        out[i] = carry;
        i++;
    }

    return i;
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
}

/*o result tem que ter espaco para aSig + bSig digitos*/
int Multiply(const unsigned long a[], unsigned long aSig,
             const unsigned long b[], unsigned long bSig,
             unsigned long result[])
{
    for (int i = 0; i < aSig + bSig; i++)
        result[i] = 0;

    //result tem que estar zerado!
    assert(result != a); //nao pode ser a mesma memoria

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
            unsigned long long ui = i < aSig ? a[i] : 0;
            unsigned long long vj = j < bSig ? b[j] : 0;
            unsigned long long rij = j > 0 ? result[i + j] : 0;
            unsigned long long current = ui * vj + rij + k;
            result[i + j] = current % DIGIT_BASE;
            k = current / DIGIT_BASE;
        }

        result[j + aSig] = k;
    }

    if (result[aSig + bSig - 1] == 0)
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

/*
   Retorna o modulo da divisao do number por um unico digito
*/
//Converte x para bigint
int  convert(unsigned long long x, unsigned long number[], int size)
{
    for (int i = 0; i < size; i++)
    {
        number[i] = x % DIGIT_BASE;
        x = x / DIGIT_BASE;
    }

    for (int i = size - 1; i >= 0; i--)
    {
        if (number[i] != 0)
            return i + 1;
    }

    return 1;
}


int remainder(unsigned long number[], int size, unsigned long value, unsigned long out[])
{
    unsigned long long carry = 0;
    for (int i = size - 1; i >= 0; i--)
    {
        carry = (carry * DIGIT_BASE + number[i]) % value;
    }
    return convert(carry, out, size);
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

    int i = leftSize > rightSize ? leftSize - 1 : rightSize - 1;

    for (; i >= 0; i--)
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

int quotient2(unsigned long number[], unsigned long size, unsigned long k,
              unsigned long out[])
{
    assert(sig_digits(number, size) == size);

    unsigned long m = sig_digits(number, size);
    unsigned long long carry = 0;

    for (int i = m - 1; i >= 0; i--)
    {
        unsigned long long temp = carry * DIGIT_BASE + number[i];
        out[i] = temp / k;
        carry = temp % k;
    }

    //limpar zeros da frente
    for (int i = m - 1; i >= 0; i--)
    {
        if (out[i] != 0)
            return i + 1;
    }
    return m;
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


unsigned long trialdigit(unsigned long r[],
                         unsigned long d[],
                         unsigned long k,
                         unsigned long m)
{
    /*
     The computation of a quotient digit qk is an iteration that
     decrements an initial estimate until it equals qk. The most
     conservative guess is b¡1, which requires O(b) corrections.
    */

    assert(m >= 2);
    assert(k + m >= m);


    unsigned long long km = k + m;
    unsigned long long r3 = (r[km] * DIGIT_BASE + r[km - 1]) * DIGIT_BASE + r[km - 2];
    unsigned long long d2 = d[m - 1] * DIGIT_BASE + d[m - 2];

    unsigned long c1 = r3 / d2;
    unsigned long c2 = DIGIT_BASE - 1;
    return c1 < c2 ? c1 : c2;
}



bool smaller(unsigned long r[], unsigned long dq[], unsigned long k, unsigned long m)
{
    int i = m;
    int j = 0;
    while (i != j)
    {
        if (r[i + k] != dq[i])
        {
            j = i;
        }
        else
            i--;
    }
    return r[i + k] < dq[i];
}

unsigned long longdiv(unsigned long left[], /*dividendo*/
                      int leftSize,  /*numero de digitos significaticos*/
                      unsigned long right[], /*divisor*/
                      int rightSize, /*numero de digitos significaticos*/
                      unsigned long out[])
{

    /*
      Exemplo 489 / 12
    */

    char buffer1[100];
    char buffer2[100];

    //printf("%s  /  %s\n", to_string_repr(left, leftSize, buffer1), to_string_repr(right, rightSize, buffer2));

    assert(sig_digits(left, leftSize) == leftSize);
    assert(sig_digits(right, rightSize) == rightSize);

    size_t n = sig_digits(left, leftSize);
    size_t m = sig_digits(right, rightSize);
    assert(m >= 2);
    assert(n >= m);


    /*
    The number of corrections can be reduced dramatically by scaling the
    operands before the division: You multiply the divisor and the dividend by
    the same digit. The scaling must make the leading digit of the divisor
    at least equal to half of the radix without changing the length of the
    divisor. This is called normalization.
    */
    unsigned long long ym_1 = right[rightSize - 1];
    unsigned long long f = DIGIT_BASE / (ym_1 + 1);

    assert(f >= 1);
    unsigned long r[1000] = { 0 };
    unsigned long rsize = MultiplyByDigit2(left, leftSize, f, r);
    n = rsize;

    unsigned long d[1000] = { 0 };
    unsigned long dsize = MultiplyByDigit2(right, rightSize, f, d);
    m = dsize;

    /*
      489  * 5      2445
      -------- =   ------
      12   * 5       60
    */

    /*limpa saida*/
    out[0] = 0;
    //printf("%s | %s\n", to_string_repr(r, rsize, buffer1), to_string_repr(d, dsize, buffer2));


    /* n=4 digits  m = 2 digits
       2 4 4 5    | 6 0
                  -------
                     <- quotient qt

    */
    for (int k = n - m; k >= 0; k--)
    {
        assert(m <= k + m);
        assert(k + m <= n);

        /*initial estimate of q[k].*/
        unsigned long  qt = trialdigit(r, d, k, m); /*qt = 4 no exemplo*/


        /*dq armazena a multiplicacao do digito de teste pelo divisor*/
        unsigned long dq[1000] = { 0 };
        unsigned int dqsize = MultiplyByDigit2(d, dsize, qt, dq);

        /*
         No exemplo:
         4 * 60 = 240
        */

        //printf("%u * %s = %s\n", qt , to_string_repr(d, dsize, buffer1), to_string_repr(dq, dqsize, buffer2));


        /*
           2445
           244 é menor que 240?
        */
        if (smaller(r, dq, k, m))
        {
            //corrige digito de teste e atualiza multiplicacao
            qt = qt - 1;
            dqsize = MultiplyByDigit2(d, dsize, qt, dq);
        }

        //printf("%s\n", to_string_repr(r, rsize, buffer1));
        /*for (int j = 0; j < (rsize - dqsize - k); j++)
        {
            printf("  ");
        }
        printf("%s ", to_string_repr(dq, dqsize, buffer2));


        printf("k=%d m=%u\n", k, m);
        */

        out[k] = qt;


        /*
            899
        */

        //difference(r, dq, k, dqsize); //k 
        /*
           Subtraimos um pedaço do r que vai de k->fim
           o dq e mantemos os valores que estao antes de k
           isso faz o equivalente a "descer" o valor conforme
           vamos precisando de mais.
        */
        subtract_in_place(&r[k], rsize - k, dq, dsize);
        //printf("%s\n", to_string_repr(r, rsize, buffer2));



        //println2(left_first, left_last);
    }
    return sig_digits(out, leftSize);
};

unsigned long longmod(unsigned long left[],
                      int leftSize,
                      unsigned long right[],
                      int rightSize,
                      unsigned long out[])
{

    /*
      Exemplo 489 / 12
    */

    char buffer1[100];
    char buffer2[100];

    //printf("%s  /  %s\n", to_string_repr(left, leftSize, buffer1), to_string_repr(right, rightSize, buffer2));

    assert(sig_digits(left, leftSize) == leftSize);
    assert(sig_digits(right, rightSize) == rightSize);

    size_t n = sig_digits(left, leftSize);
    size_t m = sig_digits(right, rightSize);
    assert(m >= 2);
    assert(n >= m);


    /*
    The number of corrections can be reduced dramatically by scaling the
    operands before the division: You multiply the divisor and the dividend by
    the same digit. The scaling must make the leading digit of the divisor
    at least equal to half of the radix without changing the length of the
    divisor. This is called normalization.
    */
    unsigned long long ym_1 = right[rightSize - 1];
    unsigned long long f = DIGIT_BASE / (ym_1 + 1);

    assert(f >= 1);
    unsigned long r[1000] = { 0 };
    unsigned long rsize = MultiplyByDigit2(left, leftSize, f, r);
    n = rsize;

    unsigned long d[1000] = { 0 };
    unsigned long dsize = MultiplyByDigit2(right, rightSize, f, d);
    m = dsize;

    /*
      489  * 5      2445
      -------- =   ------
      12   * 5       60
    */

    /*limpa saida*/
    out[0] = 0;
    //printf("%s / %s\n", to_string_repr(r, rsize, buffer1), to_string_repr(d, dsize, buffer2));

    for (int k = n - m; k >= 0; k--)
    {
        assert(m <= k + m);
        assert(k + m <= n);
        unsigned long  qt = trialdigit(r, d, k, m); /*qt = 4 no exemplo*/


        /*dq armazena a multiplicacao do digito de teste pelo divisor*/
        unsigned long dq[1000] = { 0 };
        unsigned int dqsize = MultiplyByDigit2(d, dsize, qt, dq);

        /*
         No exemplo:
         4 * 60 = 240
        */

        //printf("%u * %s = %s\n", qt , to_string_repr(d, dsize, buffer1), to_string_repr(dq, dqsize, buffer2));


        /*
           2445
           244 é menor que 240?
        */
        if (smaller(r, dq, k, m))
        {
            //corrige digito de teste e atualiza multiplicacao
            qt = qt - 1;
            dqsize = MultiplyByDigit2(d, dsize, qt, dq);
        }

        //printf("%s - %s = ", to_string_repr(r, rsize, buffer1), to_string_repr(dq, dqsize, buffer2));


        //printf("k=%d m=%u\n", k, m);

        //trabalho eh praticamente o mesmo poderia ter 2 outs
        //out[k] = qt;


        /*
            899
        */
        //difference(r, dq, k, m); //k ,
        subtract_in_place(&r[k], rsize - k, dq, dsize);

        //printf("r = %s\n", to_string_repr(r, rsize, buffer2));



        //println2(left_first, left_last);
    }
    return quotient2(r, sig_digits(r, m), f, out);
    //return sig_digits(out, leftSize);
};


/*
  tamanho de digitos do out deve ser igual a rightSize
  exemplo 20 mod 5 = 4.
  No pior caso maior mod vai ser 5-1 = 4
*/
unsigned long modulo(unsigned long left[],
                     int leftSize,
                     unsigned long right[],
                     int rightSize,
                     unsigned long out[])
{
    if (rightSize == 1)
    {
        unsigned long y1 = right[0];
        assert(y1 > 0);
        return remainder(left, leftSize, y1, out);
    }
    else
    {
        if (leftSize < rightSize)
        {
            return copy_to(left, leftSize, out);
            //assert(false);//out = left
            //out[0] = 0;
            //return 1;
        }
        else
        {
            return longmod(left, leftSize, right, rightSize, out);
        }
    }
}


unsigned long division(unsigned long left[],
                       int leftSize,
                       unsigned long right[],
                       int rightSize,
                       unsigned long out[])
{
    if (rightSize == 1)
    {
        unsigned long y1 = right[0];
        assert(y1 > 0);
        return quotient2(left, leftSize, y1, out);
    }
    else
    {
        if (leftSize < rightSize)
        {
            out[0] = 0;
            return 1;
        }
        else
        {
            return longdiv(left, leftSize, right, rightSize, out);
        }
    }
}

bool is_zero(unsigned long number[], int nsize)
{
    for (int i = 0; i < nsize; i++)
        if (number[i] != 0)
            return false;
    return true;
}



int to_dec_string(unsigned long number[], int nsize, char out[])
{

    if (is_zero(number, nsize))
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

char* to_string_repr(unsigned long number[], int nsize, char out[])
{
    char* r = out;
    for (int i = nsize - 1; i >= 0; i--)
    {
        char buffer[50];
        _itoa_s(number[i], buffer, 50, 10);
        char* p = buffer;
        while (*p != 0)
        {
            *out = *p;
            p++;
            out++;
        }
        *out = ' ';
        out++;
    }

    *out = '\0';
    out++;

    return r;
}

char* to_string(unsigned long number[], int nsize, char out[])
{



    if (nsize == 1 && number[0] == 0)
    {
        out[0] = '0';
        out[1] = '\0';
        return out;
    }

    if (nsize == 1 && number[0] == 0)
    {
        out[0] = '0';
        out[1] = '\0';
        return out;
    }

    unsigned long temp[1000];
    unsigned long tempsize = nsize;
    memcpy(temp, number, sizeof(unsigned long) * nsize);

    int i = 0;
    while (!is_zero(temp, nsize))
    {
        char ch = char(mod(temp, tempsize, 10) + '0');
        out[i] = ch;
        i++;
        tempsize = quotient(temp, tempsize, 10);
    }
    out[i] = '\0';

    for (int j = 0; j < i / 2; j++)
    {
        char temp = out[j];
        out[j] = out[i - j - 1];
        out[i - j - 1] = temp;
    }


    return out;
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

unsigned int modular_pow(unsigned long base[],
                         unsigned long baseSize,
                         unsigned long exponent[],
                         unsigned long exponentSize,
                         unsigned long modulus[],
                         unsigned long modulusSize,
                         unsigned long result[])
{
    /*
    https://en.wikipedia.org/wiki/Modular_exponentiation
    function modular_pow(base, exponent, modulus) is
    if modulus = 1 then
        return 0
    Assert :: (modulus - 1) * (modulus - 1) does not overflow base
    result := 1
    base := base mod modulus
    while exponent > 0 do
        if (exponent mod 2 == 1) then
            result := (result * base) mod modulus
        exponent := exponent >> 1
        base := (base * base) mod modulus
    return result
    */
    if (modulusSize == 1 && modulus[0] == 1)
        return 0;

    //Assert :: (modulus - 1) * (modulus - 1) does not overflow base
    //result : = 1
        //base : = base mod modulus
    baseSize = modulo(base, baseSize, modulus, modulusSize, base);

    char buffer1[1000];
    printf("%s\n", to_string_repr(base, baseSize, buffer1));

    //unsigned long result[100] = { 1 };
    result[0] = 1;
    unsigned long resultSize = 1;

    unsigned long zero[] = { 0 };
    unsigned long two[] = { 2 };
    while (compare(exponent, exponentSize, zero, 1) > 0)
    {

        unsigned long r1[100];
        unsigned long r1Size = modulo(exponent, exponentSize, two, 1, r1);

        if (r1Size == 1 && r1[0] == 1)
        {
            printf("result = %s\n", to_string_repr(result, resultSize, buffer1));
            printf("base = %s\n", to_string_repr(base, baseSize, buffer1));


            r1Size = Multiply(result, resultSize, base, baseSize, r1);
            printf("result = %s\n", to_string_repr(r1, r1Size, buffer1));

            resultSize = modulo(r1, r1Size, modulus, modulusSize, result);
            printf("result = %s\n", to_string_repr(result, resultSize, buffer1));
            //result: = (result * base) mod modulus
        }

        // x >> 1 mesmo que dividir por x / 2
        exponentSize = quotient(exponent, exponentSize, 2);

        printf("exponent = %s\n", to_string_repr(exponent, exponentSize, buffer1));
        //exponent: = exponent >> 1
        //base : = (base * base) mod modulus

        r1Size = Multiply(base, baseSize, base, baseSize, r1);
        baseSize = modulo(r1, r1Size, modulus, modulusSize, base);

        printf("base = %s\n", to_string_repr(base, baseSize, buffer1));

    }
    return sig_digits(result, baseSize);
}
```

