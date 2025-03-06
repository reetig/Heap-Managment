#include <stdio.h>
#include <stddef.h>
#define HEAPSIZE 1024

typedef struct metadata
{
    size_t size; // gives size of the free heap memory exculding metadata
    struct metadata *next;
} metadata;

char heap[HEAPSIZE];               // assuming char occupies 1 byte each
metadata *freelist = (void *)heap; // pointer towards first heap memory

void initialise_freelist()
{
    freelist->size = HEAPSIZE - sizeof(metadata);
    freelist->next = NULL;
}
// first fit
//  malloc function returns void* pointer to the first byte of the allocated memory
/*
void *my_malloc(size_t sizereq) // size requested
{
    metadata *prev = NULL;
    metadata *nptr = freelist;
    void *ret_val;

    while (nptr != NULL && nptr->size < sizereq)
    {
        prev = nptr;
        nptr = nptr->next;
    }

    if (nptr == NULL)
    {
        printf("Memory insufficient\n");
        ret_val = NULL;
    }
    else
    {
        // if we found the perfect size block
        if (nptr->size <= sizereq + sizeof(metadata))
        {
            if (prev == NULL)
            {
                // first allocated block
                freelist = freelist->next;
            }
            else
            {
                prev->next = nptr->next;
            }
            nptr->next = NULL; // the block has been allocated
        }
        else
        {
            // split the block
            metadata *newfree = (metadata *)((char *)nptr + sizeof(metadata) + sizereq);
            newfree->size = nptr->size - sizereq - sizeof(metadata);
            newfree->next = nptr->next;

            if (prev == NULL)
            {
                freelist = newfree;
            }
            else
            {
                prev->next = newfree;
            }
            nptr->size = sizereq;
            nptr->next = NULL;
        }
        ret_val = (void *)(nptr + 1);
    }
    return ret_val;
}
*/

// returns the starting address of the memory allocated excluding the metadata
void *my_malloc(size_t reqsize)
{
    metadata *prev = NULL;
    metadata *best_prev = NULL;
    metadata *nptr = freelist;
    metadata *best_block = NULL;

    void *ret_val;
    // traversing the list for best fit block
    while (nptr != NULL)
    {
        // free size is greater than the requested size
        if (nptr->size >= reqsize)
        {
            // check if this block is best fit so far
            if (best_block == NULL || nptr->size < best_block->size)
            {
                best_block = nptr;
                best_prev = prev;
            }
        }
        prev = nptr;
        nptr = nptr->next;
    }
    if (best_block == NULL)
    {
        printf("Memory insufficient.\n");
        ret_val = NULL;
    }
    else
    {
        // perfect fit or small(no splitting in this case)
        if (best_block->size <= reqsize + sizeof(metadata))
        {
            if (best_prev == NULL)
            {
                // head
                freelist = best_block->next;
            }
            else
            {
                // allocated the block without spliiting (removed from the free list)
                best_prev->next = best_block->next;
            }
            best_block->next = NULL;
        }

        else
        { // not a perfect split
            // split the block
            metadata *newfree = (metadata *)((char *)best_block + sizeof(metadata) + reqsize);
            newfree->size = best_block->size - reqsize - sizeof(metadata);
            newfree->next = best_block->next;
            if (best_prev == NULL)
            { // updating free list head
                freelist = newfree;
            }
            else
            {

                best_prev->next = newfree;
            }
            best_block->size = reqsize;
            best_block->next = NULL;
        }
        printf(" SIZE ALLOCATED : %zu ADDRESS ALLOCATED : %p \n", best_block->size, best_block + 1);
        ret_val = (void *)(best_block + 1); // skips the metadata by 1 unit of metadata so that only usable memory is seen by the user
    }
    return ret_val;
}

// free return noting so void

void my_free(void *ptr)
{
    // since any type of pointer needs to set free

    if (ptr != NULL)
    {
        metadata *block = (metadata *)ptr - 1; // points towards the metadata which is ahead of the user allocated memory
        metadata *prev = NULL;
        metadata *nptr = freelist;

        while (nptr != NULL && nptr < block) // comapres the memory addresses of the two blocks
        {
            prev = nptr;
            nptr = nptr->next;
        }
        block->next = nptr;
        if (prev == NULL)
        { // head
            freelist = block;
        }
        else
        { // insert before the nptr
            prev->next = block;
        }
        // merge the adjacent blocks

        if (block->next != NULL && ((char *)block + sizeof(metadata) + block->size) == ((char *)block->next)) // check if the memory location is contiguous
        {
            block->size += sizeof(metadata) + block->next->size;
            block->next = block->next->next;
        }
        // merging with previous block if address is contigous
        if (prev != NULL && (char *)prev + sizeof(metadata) + prev->size == (char *)block)
        {
            prev->size += sizeof(metadata) + block->size;
            prev->next = block->next;
        }
    }
}

void print_freelist()
{
    metadata *nptr = freelist;
    while (nptr != NULL)
    {
        printf("  free list is starting from address : %p  SIZE : %zu\n", nptr, nptr->size);
        nptr = nptr->next;
    }
}

int main()
{
    initialise_freelist();
    printf("initiallly the free list is :\n");
    print_freelist();
    int *a = my_malloc(sizeof(int));
    printf("free list after allocating a: ");
    print_freelist();
    char *b = my_malloc(sizeof(int));
    printf("free list after allocating b: ");
    print_freelist();
    int *c = my_malloc(sizeof(int));
    printf("free list after allocating c: ");
    print_freelist();
    float *d = my_malloc(sizeof(float));
    printf("free list after allocating d: ");
    print_freelist();

    printf("Allocated the variables now the free list looks like:\n");
    print_freelist();

    printf("Free list after freeing a\n");
    my_free(a);
    print_freelist();

    printf("Free list after freeing b\n");
    my_free(b);
    print_freelist();
    printf("Free list after freeing c\n");
    my_free(c);
    print_freelist();
    printf("Free list after freeing d\n");
    my_free(d);
    print_freelist();

    return 0;
}