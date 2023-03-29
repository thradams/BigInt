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
#include <stdint.h>
#include <stdbool.h>

#define BASE 10 //UINT32_MAX

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
            if (a->data[i] == b->data[i])
                continue;
            return a->data[i] > b->data[i] ? 1 : -1;
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

void multiply(const struct number* a, const struct number* b, struct number* result)
{
    number_reserve(result, a->digits * b->digits);
    memset(result->data, 0, result->capacity * sizeof(result->data[0]));

    int j = 0;
    for (; j < b->digits; ++j) {

        if (b->data[j] == 0)
            continue;

        uint32_t k = 0;

        for (int i = 0; i < a->digits; ++i) {
            uint64_t a_i = a->data[i];
            uint64_t b_j = b->data[j];
            uint64_t rij = j > 0 ? result->data[i + j] : 0;



            /*does not overflow because*/
            _Static_assert((uint64_t)UINT32_MAX * (uint64_t)UINT32_MAX + (uint64_t)UINT32_MAX + (uint64_t)UINT32_MAX == UINT64_MAX, "");
            uint64_t current = a_i * b_j + rij + k;

            result->data[i + j] = current % BASE;
            k = (uint32_t)(current / BASE);
        }

        result->data[j + a->digits] = k;
    }
    result->digits = a->digits + b->digits;
    for (int i = result->digits - 1; i >= 0; i--) {
        if (result->data[i] != 0)
            break;
        result->digits--;
    }
}

void add(const struct number* a, const  struct number* b, struct number* result)
{
    number_reserve(result, a->digits + b->digits + 1);//todo
    memset(result->data, 0, result->capacity * sizeof(result->data[0]));

    uint32_t k = 0;
    int max = a->digits > b->digits ? a->digits : b->digits;

    int i = 0;
    for (; i < max; i++) {
        uint64_t a_i = a->digits > i ? a->data[i] : 0;
        uint64_t b_i = b->digits > i ? b->data[i] : 0;

        /*does not overflow because*/
        _Static_assert((uint64_t)UINT32_MAX + (uint64_t)UINT32_MAX + (uint64_t)UINT32_MAX < UINT64_MAX, "");
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

uint32_t mod(const struct number* a, uint32_t value)
{
    uint64_t carry = 0;

    for (int i = a->digits - 1; i >= 0; i--) {
        carry = (carry * BASE + a->data[i]) % value;
    }

    return (uint32_t)carry;
}





struct number difference(const struct number* r, const struct number* dq, int k, int m)
{
    /*
       r[k + m..k] : = r[k + m..k] − dq[m..0]; difference: = r
    */

    struct number r2 = { 0 };
    number_reserve(&r2, r->digits);
    memset(r2.data, 0, r->digits * sizeof(r->data[0]));

    //{ 0 <= k <= k + m <= w }
    uint32_t borrow = 0;
    for (int i = 0; i < m; i++) {

        uint32_t diff = r->data[i + k] - dq->data[i] - borrow + BASE;
        r->data[i + k] = diff % BASE;
        borrow = 1 - diff / BASE;
    }

    //assume borrow = 0;
    //difference: = r
    return r2;
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
        uint64_t temp = a->data[i];
        temp *= value;
        temp += carry;

        a->data[i] = temp % BASE;
        carry = temp / BASE;
    };

    if (carry != 0) {
        number_reserve(a, a->digits + 1);
        a->data[i] = (uint32_t)carry;
        a->digits++;
    }
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
        a->data[i] = (uint32_t)temp / value;
        carry = temp % value;
    }

    for (int i = a->digits - 1; i >= 0; i--) {
        if (a->data[i] != 0)
            break;
        a->digits--;
    }
}


struct number number_dup(const struct number* a) {

    struct number r = { 0 };
    number_reserve(&r, a->digits);
    r.digits = a->digits;
    memcpy(r.data, a->data, sizeof(a->data[0]) * a->digits);
    return r;
}


int smaller(const struct number* r, const struct number* dq, int k, int m)
{
    //{ r[k + m..k] < dq[m..0] };
    //{ 0 <= k <= k + m <= w }
    int i = m;
    int j = 0;
    while (i != j) {
        if (r->data[i + k] != dq->data[i])
            j = i;
        else
            i = i - 1;
    }
    return r->data[i + k] < dq->data[i];
}

uint32_t trialdigit(const struct number* r, const struct number* d, uint32_t k, uint32_t m)
{
    uint32_t d2, km, r3;
    //{ 2 <= m <= k + m <= w }
    km = k + m;
    r3 = (r->data[km] * BASE + r->data[km - 1]) * BASE + r->data[km - 2];
    d2 = d->data[m - 1] * BASE + d->data[m - 2];
    return (r3 / d2) < (BASE - 1) ? r3 / d2 : BASE - 1;
}

struct number longdiv(struct number* x, const struct number* y)
{
    assert(x->digits >= y->digits);


    int n = x->digits;
    int m = y->digits;

    assert(2 <= m && m <= n);
    //{ longdiv = x div y }
    //var d, dq, q, r: number;
    //f, k, qt: integer;

    //{ 2 <= m <= n <= w }
    uint32_t  f = BASE / (y->data[m - 1] + 1);

    struct number r = { 0 };
    r = number_dup(x);
    multiply_in_place(&r, f);

    number_print(&r);

    struct number d = { 0 };
    d = number_dup(y);
    multiply_in_place(&d, f);

    number_print(&d);

    struct number q = { 0 };
    number_reserve(&q, x->digits);
    memset(q.data, 0, x->digits * sizeof(x->data[0]));

    number_print(&q);

    for (int k = n - m; k >= 0; k--)
    {
        //{ 2 <= m <= k + m <= n <= w }
        uint32_t qt = trialdigit(&r, &d, k, m);

        struct number dq = number_dup(&d);
        multiply_in_place(&dq, qt);

        number_print(&dq);

        if (smaller(&r, &dq, k, m)) {
            qt = qt - 1;

            number_destroy(&dq);
            dq = number_dup(&d);
            multiply_in_place(&dq, qt);
            number_print(&dq);

        }
        q.data[k] = qt;
        r = difference(&r, &dq, k, m);
        number_print(&r);
    }
    //longdiv: = q
    return q;
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

void number_set(struct number* a, uint64_t x)
{
    a->digits = 0;
    a->data[0] = 0;


    for (;;)
    {
        number_push_back(a, x % BASE);
        x = x / BASE;
        if (x == 0)
            break;
    }

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

    struct number a = parse_number("100");//4294967295 + 1
    number_print(&a);
    //number_set(&a, 165465465465465424);
    //number_print(&a);

    struct number b = parse_number("24");
    number_print(&b);

    struct number r = longdiv(&a, &b, &r);
    number_print(&r);

    struct number base10 = to_base10(&r);
    number_print(&base10);

}
```
