#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef USE_SYSTEM_MALLOC
    #define xmalloc malloc
    #define xfree free
    #define xrealloc realloc
#else
    #include "allocator.h"
#endif

void test_basic_allocation();
void test_multiple_allocations();
void test_free_and_reallocate();
void test_alloc_free_alloc();
void test_reallocation();
void test_exceed_heap_capacity();

typedef void (*test_func)();

typedef struct {
    const char *name;
    test_func func;
} test_t;

test_t tests[] = {
    {"Basic Allocation Test", test_basic_allocation},
    {"Multiple Allocations Test", test_multiple_allocations},
    {"Free and Reallocate Test", test_free_and_reallocate},
    {"Alloc/Free/Alloc Test", test_alloc_free_alloc},
    {"Reallocation Test", test_reallocation},
    {"Exceed Heap Capacity Test", test_exceed_heap_capacity}
};

#define NUM_TESTS (sizeof(tests) / sizeof(tests[0]))

void test_basic_allocation() {
    char *ptr = xmalloc(16);
    if (ptr) {
        printf("Basic Allocation Test: PASS\n");
    } else {
        printf("Basic Allocation Test: FAIL\n");
    }
    xfree(ptr);
}

void test_multiple_allocations() {
    char *ptr1 = xmalloc(16);
    char *ptr2 = xmalloc(32);
    char *ptr3 = xmalloc(64);

    if (ptr1 && ptr2 && ptr3 && ptr2 > ptr1 && ptr3 > ptr2) {
        printf("Multiple Allocations Test: PASS\n");
    } else {
        printf("Multiple Allocations Test: FAIL\n");
    }

    xfree(ptr1);
    xfree(ptr2);
    xfree(ptr3);
}

void test_free_and_reallocate() {
    char *ptr = xmalloc(16);
    xfree(ptr);
    char *new_ptr = xmalloc(16);

    if (new_ptr == ptr) {
        printf("Free and Reallocate Test: PASS\n");
    } else {
        printf("Free and Reallocate Test: FAIL\n");
    }
    xfree(new_ptr);
}

void test_alloc_free_alloc() {
    char *ptr1 = xmalloc(16);
    char *ptr2 = xmalloc(32);
    xfree(ptr1);
    char *ptr3 = xmalloc(16);

    if (ptr3 == ptr1) {
        printf("Interleaved Alloc/Free Test: PASS\n");
    } else {
        printf("Interleaved Alloc/Free Test: FAIL\n");
    }

    xfree(ptr2);
    xfree(ptr3);
}

void test_reallocation() {
    char *ptr = xmalloc(16);
    strcpy(ptr, "Hello");
    char *new_ptr = xrealloc(ptr, 32);

    if (new_ptr && strcmp(new_ptr, "Hello") == 0) {
        printf("Reallocation Test: PASS\n");
    } else {
        printf("Reallocation Test: FAIL\n");
    }
    xfree(new_ptr);
}

void test_exceed_heap_capacity() {
    char *ptr = xmalloc(2 * 1024 * 1024);
    if (!ptr) {
        printf("Exceed Heap Capacity Test: PASS\n");
    } else {
        printf("Exceed Heap Capacity Test: FAIL\n");
    }
}


int main(int argc, char *argv[]) {
    if (argc == 3 && strcmp(argv[1], "-t") == 0) {
        int test_num = atoi(argv[2]);
        if (test_num >= 0 && test_num < NUM_TESTS) {
            tests[test_num].func();
        } else {
            printf("Invalid test number.\n");
        }
    } else {
        for (int i = 0; i < NUM_TESTS; i++) {
            printf("Running %s...\n", tests[i].name);
            tests[i].func();
        }
    }

    return 0;
}
