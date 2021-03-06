
#include "BigInt.h"



void TestAdd()
{
    Digit a[] = { LAST_DIGIT };
    Digit b[] = { 1 };
    Digit r[] = { 0, 0 };

    //result must be at least one digit bigger
    //than the significative digits or a and b.
    int digits = Add(a, 1, b, 1, r);
    assert(digits == 2);
    assert(r[0] == 0);
    assert(r[1] == 1);
}
void TestSubtract()
{
    Digit a[] = { 0, 1 };
    Digit b[] = { 1 };
    Digit r[] = { 0, 0 };
    int nDigits = Subtract(a, 2, b, 1, r);
    assert(nDigits == 1);
}

void TestMultiply()
{
    Digit a[] = { 6, 1 };
    Digit b[] = { 2 };
    Digit r[] = { 0, 0 , 0 };
    Multiply(a, 2, b, 1, r);
}


void TestParse2()
{
    Digit digits[10] = { 0 };
    int digitsLen = sizeof(digits) / sizeof(digits[0]);

    int d = Parse("0012", digits, digitsLen);
#ifdef BASE10
    assert(d == 2);
    assert(digits[1] == 1 && digits[0] == 2);
#else
    assert(d == 1);
    assert(digits[0] == 12);
#endif
}


void TestParse()
{
    Digit digits[10] = { 0 };
    int digitsLen = sizeof(digits) / sizeof(digits[0]);

    int d = Parse("123", digits, digitsLen);
#ifdef BASE10
    assert(d == 3);
    assert(digits[2] == 1 && digits[1] == 2 && digits[0] == 3);
#else
    assert(d == 1);
    assert(digits[0] == 123);
#endif
}

int main()
{
    TestParse();
    TestParse2();

    TestAdd();
    TestSubtract();
    TestMultiply();
}