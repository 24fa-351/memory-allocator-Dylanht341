#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "allocator.h"

#define HEAP_SIZE 1024 * 1024
#define MAX_BLOCKS 1024

typedef struct block_header {
    size_t size;          // Size of the block
    int isFree;             // 1 = free, 0 = allocated
    struct block_header* next;
} block_header_t;

static block_header_t* heap[MAX_BLOCKS];
static int heap_size = 0;
static void* heap_start = NULL;

// Functions to maintain heap
void minheap_insert(block_header_t* block);
block_header_t* minheap_get_min();
void heap_up(int index);
void heap_down(int index);

void initialize_heap() {
    if (heap_start != NULL) {
        return;
    }

    heap_start = sbrk(HEAP_SIZE);
    if (heap_start == (void*)-1) {
        fprintf(stderr, "sbrk failed\n");
        exit(1);
    }

    block_header_t* initial_block = (block_header_t*)heap_start;
    initial_block->size = HEAP_SIZE - sizeof(block_header_t);
    initial_block->isFree = 1;
    initial_block->next = NULL;

    minheap_insert(initial_block);
}

char* xmalloc(size_t size) {
    size = (size + 7) & ~7;
    block_header_t* smallest_block = NULL;

    if (heap_start == NULL) {
        initialize_heap();
    }

    while (heap_size > 0) {
        block_header_t* block = minheap_get_min();
        if (block->size >= size) {
            smallest_block = block;
            break;
        }
    }

    if (smallest_block == NULL) {
        fprintf(stderr, "No block found for size %lu\n", size);
        return NULL;
    }

    // Split block if larger than requested size
    if (smallest_block->size > size + sizeof(block_header_t)) {
        block_header_t* new_block = (block_header_t*)((char*)smallest_block + sizeof(block_header_t) + size);
        new_block->size = smallest_block->size - size - sizeof(block_header_t);
        new_block->isFree = 1;

        minheap_insert(new_block);
        smallest_block->size = size;
    }

    smallest_block->isFree = 0;
    return (char*)(smallest_block + 1);
}

void xfree(void* ptr) {
    block_header_t* block = (block_header_t*)ptr - 1;
    block->isFree = 1;
    
    if (ptr == NULL) {
        return;
    }
    
    minheap_insert(block);
}

void* xrealloc(void* ptr, size_t size) {
    block_header_t* block = (block_header_t*)ptr - 1;
    
    if (ptr == NULL) {
        return xmalloc(size);
    }

    if (block->size >= size) {
        return ptr;
    }

    char* new_ptr = xmalloc(size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, block->size);
        xfree(ptr);
    }
    return new_ptr;
}

void minheap_insert(block_header_t* block) {
    if (heap_size >= MAX_BLOCKS) {
        fprintf(stderr, "minheap overflow\n");
        exit(1);
    }
    heap[heap_size] = block;
    heap_up(heap_size);
    heap_size++;
}

block_header_t* minheap_get_min() {
    if (heap_size == 0) {
        return NULL;
    }

    block_header_t* min = heap[0];
    heap[0] = heap[--heap_size];
    heap_down(0);

    return min;
}

void heap_up(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;

        if (heap[index]->size >= heap[parent]->size) {
            break;
        }

        block_header_t* temp = heap[index];
        heap[index] = heap[parent];
        heap[parent] = temp;
        index = parent;
    }
}

void heap_down(int index) {
    while (1) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;

        if (left < heap_size && heap[left]->size < heap[smallest]->size) {
            smallest = left;
        }

        if (right <  heap_size && heap[right]->size < heap[smallest]->size) {
            smallest = right;
        }

        if (smallest == index) {
            break;
        }

        block_header_t* temp = heap[index];
        heap[index] = heap[smallest];
        heap[smallest] = temp;

        index = smallest;
    }
}