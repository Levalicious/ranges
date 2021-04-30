#include <stdio.h>
#include "range.h"

int cmpfunc(const void* a, const void* b) {
    if (*((u64*) a) > *((u64*) b)) return 1;
    if (*((u64*) a) < *((u64*) b)) return -1;
    return 0;
}

void addSet(range* a, range* b) {
    u64 asize = (a->e[0] - a->b[0]) / a->p[0];
    u64 bsize = (b->e[0] - b->b[0]) / b->p[0];
    asize += 1;
    bsize += 1;
    u64* arr = calloc(1, sizeof(u64) * asize * bsize);
    u64 ai, bi;
    ai = bi = 0;
    for (u64 i = a->b[0]; i <= a->e[0]; i += a->p[0]) {
        bi = 0;
        for (u64 j = b->b[0]; j <= b->e[0]; j += b->p[0]) {
            arr[ai * bsize + bi] = i + j;
            bi++;
        }
        ai++;
    }

    qsort(arr, asize * bsize, sizeof(u64), cmpfunc);
    printf("%lu, ", arr[0]);
    for (u64 i = 1; i < asize * bsize; ++i) {
        if (arr[i] == arr[i - 1]) continue;
        printf("%lu, ", arr[i]);
    }
    printf("\n");
    printf("Dist:\n");
    for (u64 i = 1; i < asize * bsize; ++i) {
        if (arr[i] == arr[i - 1]) continue;
        printf("%lu, ", arr[i] - arr[i - 1]);
    }
    printf("\n");
    for (u64 i = (asize * bsize) - 1; i > 0; --i) {
        if (arr[i] == arr[i - 1]) continue;
        printf("%lu, ", arr[i] - arr[i - 1]);
    }
    printf("\n");
    free(arr);
}

void multSet(range* a, range* b) {
    u64 asize = (a->e[0] - a->b[0]) / a->p[0];
    u64 bsize = (b->e[0] - b->b[0]) / b->p[0];
    asize += 1;
    bsize += 1;
    u64* arr = malloc(sizeof(u64) * asize * bsize);
    u64 ai, bi;
    ai = bi = 0;
    for (u64 i = a->b[0]; i <= a->e[0]; i += a->p[0]) {
        bi = 0;
        for (u64 j = b->b[0]; j <= b->e[0]; j += b->p[0]) {
            arr[ai * bsize + bi] = i * j;
            bi++;
        }
        ai++;
    }

    qsort(arr, asize * bsize, sizeof(u64), cmpfunc);
    printf("%lu, ", arr[0]);
    for (u64 i = 1; i < asize * bsize; ++i) {
        if (arr[i] == arr[i - 1]) continue;
        printf("%lu, ", arr[i]);
    }
    printf("\n");
    printf("Dist:\n");
    for (u64 i = 1; i < asize * bsize; ++i) {
        if (arr[i] == arr[i - 1]) continue;
        printf("%lu, ", arr[i] - arr[i - 1]);
    }
    printf("\n");
    for (u64 i = asize * bsize; i > 0; ++i) {
        if (arr[i] == arr[i - 1]) continue;
        printf("%lu, ", arr[i] - arr[i - 1]);
    }
    printf("\n");
    free(arr);
}

u64 smin(range* a) {
    if (a->b[0] != 0) return a->b[0];
    if (a->b[0] + a->p[0] < a->e[0]) return a->b[0] + a->p[0];
    return UINT64_MAX;
}

u64 smax(range* a) {
    u64 j = (a->e[0] - a->b[0]) / a->p[0];
    j *= a->p[0];
    j += a->b[0];
    return j;
}



int main() {
    range* a = initRange(3, 15, 4);
    range* b = initRange(2, 20, 7);
    range* c = initRange(0, 0, 0);
    subRange(c, a, b);

    printRange(a);
    printRange(b);
    printRange(c);

    printf("%lu\n", a->b[0] - b->e[0]);
    printf("%lu\n", b->b[0] - a->e[0]);

    return 0;
}
