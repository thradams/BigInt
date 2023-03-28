# BigInt
BigInt

## References
* http://www.brinch-hansen.net/papers/1994b.pdf


Attention code incomplete.


```c




#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include < stdint.h >
#include <stdbool.h>

#define BASE UINT32_MAX

struct number {
    uint32_t* data;
    int digits;
    int capacity;
};

void number_print(struct number* a)
{
    if (a->digits == 0)
        printf("zero");

    for (int i = a->digits - 1; i >= 0; i--) {
        printf("%u ", a->data[i]);
    }
    printf("\n");
}

void number_swap(struct number* a, struct number* b)
{
    struct number tmp = *a;
    *a = *b;
    *b = tmp;
}

int number_reserve(struct number* p, int n)
{
    if (n > p->capacity) {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0])))) {
            return EOVERFLOW;
        }

        void* pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;

        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}


int compare(struct number* a, struct number* b)
{
    if (a->digits == b->digits)
    {
        for (int i = 0; i < a->digits; i++) {
            if (a->data[i] == b->data)
                continue;
            return a->data[i] > b->data ? 1 : -1;
        }
        return 0;
    }


    return a->digits > b->digits ? 1 : -1;
}


int number_push_back(struct number* p, int value)
{
    if (p->digits == INT_MAX) {
        return EOVERFLOW;
    }

    if (p->digits + 1 > p->capacity) {
        int new_capacity = 0;
        if (p->capacity > (INT_MAX - p->capacity / 2))
        {
            /*overflow*/
            new_capacity = INT_MAX;
        }
        else {
            new_capacity = p->capacity + p->capacity / 2;
            if (new_capacity < p->digits + 1) {
                new_capacity = p->digits + 1;
            }
        }

        int error = number_reserve(p, new_capacity);
        if (error != 0) {
            return error;
        }
    }

    p->data[p->digits] = value;


    p->digits++;

    return 0;
}

void number_destroy(struct number* p)
{
    free(p->data);
}

void multiply(struct number* a, struct number* b, struct number* result)
{
    number_reserve(result, a->digits * b->digits);
    memset(result->data, 0, result->capacity * sizeof(result->data[0]));

    int j = 0;
    for (; j < b->digits; ++j) {

        if (b->data[j] == 0)
            continue;

        uint32_t k = 0;

        for (size_t i = 0; i < a->digits; ++i) {
            uint64_t a_i = a->data[i];
            uint64_t b_j = b->data[j];
            uint64_t rij = j > 0 ? result->data[i + j] : 0;
            uint64_t current = a_i * b_j + rij + k;
            result->data[i + j] = current % BASE;
            k = (uint32_t)(current / BASE);
        }

        result->data[j + a->digits] = k;
    }

    for (int i = result->digits - 1; i >= 0; i--) {
        if (result->data[i] != 0)
            break;
        result->digits--;
    }
}

void add(struct number* a, struct number* b, struct number* result)
{
    number_reserve(result, a->digits + b->digits + 1);//todo
    memset(result->data, 0, result->capacity * sizeof(result->data[0]));

    uint32_t k = 0;
    int max = a->digits > b->digits ? a->digits : b->digits;

    int i = 0;
    for (; i < max; i++) {
        uint64_t a_i = a->digits > i ? a->data[i] : 0;
        uint64_t b_i = b->digits > i ? b->data[i] : 0;

        uint64_t current = a_i + b_i + k;
        result->data[i] = current % UINT_MAX;
        k = (uint32_t)(current / UINT_MAX);
    }

    assert(k == 0 || k == 1);

    if (k > 0) {
        result->data[i] = k;
        i++;
    }

    result->digits = i;
}



void reverse(uint32_t* first, uint32_t* last)
{
    if (first == last)
        return;

    for (--last; first < last; ++first, --last)
    {
        uint32_t temp = *first;
        *first = *last;
        *last = temp;
    }
}

uint32_t mod(struct number* a, uint32_t value)
{
    uint64_t carry = 0;

    for (int i = a->digits - 1; i >= 0; i--) {
        carry = (carry * BASE + a->data[i]) % value;
    }

    return (uint32_t)carry;
}



unsigned long long trialdigit2(unsigned long long r,
    unsigned long long d,
    size_t k,
    size_t m)
{
    typedef unsigned long long ulong;
    // r{3} = r[k + m .. k + m - 2]
    // d{2} = d[m - 1..m-2]
    size_t km = k + m;
    double rkm = r[km];
    double rkm_1 = r[km - 1];
    double rkm_2 = r[km - 2];
    double dm_1 = d[m - 1];
    double dm_2 = d[m - 2];
    double r3 = (rkm * BASE + rkm_1) * BASE + rkm_2;
    double d2 = dm_1 * BASE + dm_2;

    double r4 = r3 / d2;
    if (r4 < (double)(BASE - 1))
        return r4;

    return (double)(BASE - 1);
}


struct number difference(struct number* r, struct number* dq, int k, int m)
{
  /*
     r[k + m..k] : = r[k + m..k] − dq[m..0]; difference: = r 
  */
   
    struct number r = { 0 };
   //{ 0 <= k <= k + m <= w }
   uint32_t borrow = 0;
   for (int i = 0; i < m; i++) {

       uint32_t diff = r.data[i + k] − dq->data[i] − borrow + BASE;
       r.data[i + k] = diff % b;
       borrow = 1 − diff / BASE;
   }

   //assume borrow = 0;
   //difference: = r
   return r;
}

void multiply_in_place(struct number* a, uint32_t value)
{
    if (value == 0) {
        a->data[0] = 0;
        a->digits = 1;
        return;
    }

    unsigned long long carry = 0;

    int i = 0;
    for (; i < a->digits; i++)
    {
        unsigned long long temp = a->data[i];
        temp *= value;
        temp += carry;
        a->data[i] = temp % BASE;
        carry = temp / BASE;
    };

    if (carry != 0) {
        number_reserve(a, a->digits + 1);
        a->data[i] = carry;
        a->digits++;
    }
};

void longdiv(struct number* a,
    struct number* b,
    struct number* r)
{

    const size_t n = a->digits;
    const size_t m = b->digits;

    typedef unsigned long  long ulong;

    //RightIterator sigvlast = sig_digit_end(right_first, right_last);
    ulong ym_1 = b->data[b->digits - 1];
    ulong f = BASE / (ym_1 + 1);

    multiply_in_place(&a, f);
    multiply_in_place(&b, f);

    for (int k = n - m; k >= 0; k--)
    {
        ulong qt = trialdigit2(a->data[0], b->data[0], (size_t)k, (size_t)m);

        struct number dq = { 0 };
        number_reserve(&dq, b->digits + 1);
        memset(dq.data, 0, sizeof(dq.data[0]) * b->digits + 1);


        multiply(right_first, right_last, qt, dq.begin(), dq.end());

        if (compare(left_first + k, left_first + k + m,
            dq.begin(), dq.begin() + m) == -1)
        {
            qt = qt - 1;
            multiply<base>(left_first, left_last, qt, dq.begin(), dq.end());
        }

        out_first[k] = qt;
        subtract_in_place<base>(left_first + k, left_first + k + m, dq.begin(), dq.begin() + m);
    }

    //fill leading zeroes
    //for (size_t i = n - m + 1; i < out_last - out_first; i++)
    //{
      //  out_first[i] = 0;
    //}
};



void divide_in_place(struct number* a, uint32_t value)
{
    if (value == 0) {
        return;
    }

    uint64_t carry = 0;

    for (int i = a->digits - 1; i >= 0; i--) {
        uint64_t ui = a->data[i];
        uint64_t temp = carry * BASE + ui;
        a->data[i] = temp / value;
        carry = temp % value;
    }

    for (int i = a->digits - 1; i >= 0; i--) {
        if (a->data[i] != 0)
            break;
        a->digits--;
    }
}



struct number parse_number(const char* psz)
{
    struct number r = { 0 };

    number_reserve(&r, 1);
    r.data[0] = 0;
    r.digits = 1;

    struct number result2 = { 0 };


#if BASE == 10
    struct number ten = {
            .data = (uint32_t[2]) { 0, 1 }, .digits = 2, .capacity = 2
    };
#else
    struct number ten = {
            .data = &(uint32_t) { 10 }, .digits = 1, .capacity = 1
    };
#endif

    number_print(&ten);

    struct number d = {
        .data = &(uint32_t) { 0 }, .digits = 1, .capacity = 1
    };

    while ((*psz >= '0') && (*psz <= '9'))
    {
        multiply(&r, &ten, &result2);

        number_print(&result2);

        d.data[0] = (*psz - '0');
        number_swap(&r, &result2);
        add(&r, &d, &result2);

        number_swap(&r, &result2);
        number_print(&r);
        psz++;
    }

    return r;
}

bool number_is_zero(const struct number* a) {
    return a->digits == 0 ||
        (a->digits == 1 && a->data[0] == 0);
}

struct number  to_base10(struct number* number) {

    struct number base10 = { 0 };
    struct number copy = { 0 };
    number_reserve(&copy, number->digits);
    copy.digits = number->digits;
    copy.capacity = number->digits;
    memcpy(copy.data, number->data, sizeof(number->data[0]) * number->digits);

    struct number r = { 0 };

    while (!number_is_zero(&copy))
    {
        uint32_t m = mod(&copy, 10); //digit
        number_push_back(&base10, m);
        divide_in_place(&copy, 10);
    }
    reverse(&base10.data[0], &base10.data[base10.digits - 1]);
    return base10;
}

int main()
{

    struct number a = parse_number("4294967296");//4294967295 + 1
    number_print(&a);

    struct number b = parse_number("34");
    number_print(&b);

    struct number r = { 0 };
    multiply(&a, &b, &r);
    number_print(&r);

    struct number base10 = to_base10(&r);
    number_print(&base10);

}
```
