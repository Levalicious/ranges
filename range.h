#pragma once
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    u64* p;
    u64* b;
    u64* e;
    u64 cnt;
    u64 cap;
} range;

range* initRange(u64 b, u64 e, u64 p) {
    range* o = malloc(sizeof(range));
    if (o == NULL) {
        printf("Failed to allocate range\n");
        exit(0);
    }

    o->cnt = 0;
    o->cap = 16;
    o->p = malloc(sizeof(u64) * o->cap);
    o->b = malloc(sizeof(u64) * o->cap);
    o->e = malloc(sizeof(u64) * o->cap);
    if (o->p == NULL || o->b == NULL || o->e == NULL) {
        printf("Failed to allocate range arrays\n");
        exit(0);
    }

    o->cnt = 1;
    o->p[0] = p;
    o->b[0] = b;
    o->e[0] = e;

    return o;
}

void list10(range* r) {
    for (u64 i = 0; i < 9; ++i) {
        printf("%lu, ", r->b[0] + r->p[0] * i);
    }
    printf("%lu\n", r->b[0] + r->p[0] * 9);
}

void printRange(range* r) {
    for (u64 i = 0; i < r->cnt; ++i) {
        printf("%lu %lu %lu\n", r->b[i], r->e[i], r->p[i]);
    }
    printf("\n");
}


void reallocrange(range* c) {
    c->cap *= 2;

    c->p = realloc(c->p, sizeof(u64) * c->cap);
    c->b = realloc(c->b, sizeof(u64) * c->cap);
    c->e = realloc(c->e, sizeof(u64) * c->cap);
    if (c->p == NULL || c->b == NULL || c->e == NULL) {
        printf("Failed to realloc range\n");
        exit(0);
    }
}

range* irange(range* a, u64 b, u64 e, u64 p) {
    if (a->cnt >= a->cap) reallocrange(a);
    a->b[a->cnt] = b;
    a->e[a->cnt] = e;
    a->p[a->cnt] = p;
    a->cnt++;
    return a;
}

u64 gcd(u64 u, u64 v) {
    i64 shift;
    if (u == 0LU) return v;
    if (v == 0LU) return u;
    shift = __builtin_ctzll(u | v);
    u >>= __builtin_ctzll(u);
    do {
        v >>= __builtin_ctzll(v);
        if (u > v) {
            u64 t = v;
            v = u;
            u = t;
        }
        v = v - u;
    } while (v != 0);
    return u << shift;
}

range* sortRange(range* a) {
    for (u64 i = 0; i < a->cnt - 1; ++i) {
        for (u64 j = 0; j < a->cnt - i - 1; ++j) {
            if (a->p[j] > a->p[j + 1]) {
                u64 b, e, p;
                b = a->b[j];
                e = a->e[j];
                p = a->p[j];
                a->b[j] = a->b[j + 1];
                a->e[j] = a->e[j + 1];
                a->p[j] = a->p[j + 1];
                a->b[j + 1] = b;
                a->e[j + 1] = e;
                a->p[j + 1] = p;
            }
        }
    }
    return a;
}

range* reduceRange(range* a) {
    sortRange(a);
    u64 ncnt = a->cnt;
    for (u64 i = a->cnt - 1; i < a->cnt; --i) {
        for (u64 j = 0; j < i; ++j) {
            /* Case: Same range, larger period is multiple of smaller */
            if (a->b[i] == a->b[j] && a->e[i] == a->e[j] && a->p[i] % a->p[j] == 0) {
                a->p[i] = UINT64_MAX;
                ncnt--;
                break;
            }
            /* Case: Same period, same phase, overlapping/touching */
            if (a->p[i] == a->p[j] && (a->b[i] % a->p[i] == a->b[j] % a->p[j])) {
                u64 mn, mi, mb, me;
                mn = min(a->e[i], a->e[j]);
                mb = max(a->b[i], a->b[j]);
                me = max(a->e[i], a->e[j]);
                mi = min(a->b[i], a->b[j]);

                u64 nval = (((mn - mi) / a->p[i]) + 1) * a->p[i] + mi;

                if (nval >= mb && nval <= me) {
                    a->b[j] = mi;
                    a->e[j] = me;
                    ncnt--;
                    break;
                }
            }
        }
    }
    sortRange(a);
    a->cnt = ncnt;
    return a;
}

range* rangeRenorm(range* a) {
    for (u64 i = 0; i < a->cnt; ++i) {
        u64 nval = (((a->e[i] - a->b[i]) / a->p[i])) * a->p[i] + a->b[i];
        a->e[i] = nval;
    }
    return a;
}

range* addRange(range* c, range* a, range* b) {
    rangeRenorm(a);
    rangeRenorm(b);
    tryAgain:;
    c->cnt = 2 * a->cnt * b->cnt;
    if (c->cnt >= c->cap) {
        reallocrange(c);
        goto tryAgain;
    }
    for (u64 i = 0; i < a->cnt; ++i) {
        for (u64 j = 0; j < b->cnt; ++j) {
            c->p[i * b->cnt + j * 2 + 0] = a->p[i];
            c->b[i * b->cnt + j * 2 + 0] = a->b[i] + b->b[j];
            if (c->p[i * b->cnt + j * 2 + 0] != 1) {
                c->e[i * b->cnt + j * 2 + 0] = (((a->e[i] + b->e[j]) - c->b[i * b->cnt + j * 2 + 0]) / a->p[i]) * a->p[i] + c->b[i * b->cnt + j * 2 + 0];
            } else {
                c->e[i * b->cnt + j * 2 + 0] = a->e[i] + b->e[j];
            }

            c->p[i * b->cnt + j * 2 + 1] = b->p[j];
            c->b[i * b->cnt + j * 2 + 1] = a->b[i] + b->b[j];
            if (c->p[i * b->cnt + j * 2 + 0] != 1) {
                c->e[i * b->cnt + j * 2 + 1] = (((a->e[i] + b->e[j]) - c->b[i * b->cnt + j * 2 + 1]) / a->p[j]) * a->p[j] + c->b[i * b->cnt + j * 2 + 1];
            } else {
                c->e[i * b->cnt + j * 2 + 1] = a->e[i] + b->e[j];
            }
        }
    }

    return reduceRange(c);;
}

range* subRange(range* c, range* a, range* b) {
    rangeRenorm(a);
    rangeRenorm(b);
    tryAgain:;
    c->cnt = 2 * a->cnt * b->cnt;
    if (c->cnt >= c->cap) {
        reallocrange(c);
        goto tryAgain;
    }
    for (u64 i = 0; i < a->cnt; ++i) {
        for (u64 j = 0; j < b->cnt; ++j) {
            c->p[i * b->cnt + j * 2 + 0] = a->p[i];
            c->b[i * b->cnt + j * 2 + 0] = a->b[i] - b->e[j];
            if (c->p[i * b->cnt + j * 2 + 0] != 1) {
                c->e[i * b->cnt + j * 2 + 0] = (((a->e[i] - b->b[j]) - c->b[i * b->cnt + j * 2 + 0]) / a->p[i]) * a->p[i] + c->b[i * b->cnt + j * 2 + 0];
            } else {
                c->e[i * b->cnt + j * 2 + 0] = a->e[i] - b->b[j];
            }

            c->p[i * b->cnt + j * 2 + 1] = b->p[j];
            c->b[i * b->cnt + j * 2 + 1] = a->b[i] - b->e[j];
            if (c->p[i * b->cnt + j * 2 + 0] != 1) {
                c->e[i * b->cnt + j * 2 + 1] = (((a->e[i] - b->b[j]) - c->b[i * b->cnt + j * 2 + 1]) / a->p[j]) * a->p[j] + c->b[i * b->cnt + j * 2 + 1];
            } else {
                c->e[i * b->cnt + j * 2 + 1] = a->e[i] - b->b[j];
            }
        }
    }

    return reduceRange(c);;
}

void freeRange(range* r) {
    free(r->p);
    free(r->b);
    free(r->e);
    free(r);
}