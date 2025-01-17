#include <stdlib.h>
#include <stdio.h>

#define SYSTEM_MEMORY 4096

typedef struct __node_t {
    int size;
    int isFree; 
    struct __node_t* next;
} node_t;

typedef struct {
    node_t* freeList;
} heap_t;

heap_t* init_heap() {
    heap_t* heap = (heap_t*) malloc(sizeof(heap_t));
    heap->freeList = (node_t*) malloc(sizeof(node_t));
    heap->freeList->size = SYSTEM_MEMORY;
    heap->freeList->isFree = 1;
    heap->freeList->next = NULL;
    return heap;
}

node_t* allocate_memory(heap_t* heap, int size) {
    node_t* curr = heap->freeList;
    while (curr) {
        if (curr->isFree && curr->size >= size) {
            int leftover = curr->size - size;
            curr->size = size;
            curr->isFree = 0;

            if (leftover > 0) {
                node_t* newNode = (node_t*) malloc(sizeof(node_t));
                newNode->size = leftover;
                newNode->isFree = 1;
                newNode->next = 0;
                curr->next = newNode;
            }
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void free_memory(node_t* node) {
    if (!node || node->isFree) return;

    node->isFree = 1;

    while (node->next && node->next->isFree) {
        node->size += node->next->size;
        node_t* tmp = node->next;
        node->next = node->next->next;
        free(tmp);
    }
}

void display_heap(heap_t* heap) {
    printf("Free List: ");
    node_t* curr = heap->freeList;
    while (curr) {
        printf("%d(%s) -> ", curr->size, curr->isFree ? "free" : "alloc");
        curr = curr->next;
    }
    printf("NULL\n");
}

void free_heap(heap_t* heap) {
    node_t* curr = heap->freeList;
    while (curr) {
       node_t* tmp = curr->next; 
       free(curr);
       curr = tmp;
    }
    free(heap);
}

int main() {
    heap_t* heap = init_heap();

    node_t* mem1 = allocate_memory(heap, 100);
    node_t* mem2 = allocate_memory(heap, 500);
    free_memory(mem2);
    node_t* mem3 = allocate_memory(heap, 420);
    free_memory(mem1);
    node_t* mem4 = allocate_memory(heap, 420);

    display_heap(heap);

    free_heap(heap);
}

