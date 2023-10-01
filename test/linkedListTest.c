#include <stdio.h>
#include <assert.h>
#include <time.h>

/*****************************TEMPLATE INCLUDE**************************************/
// #define LINKED_LIST_BUFFER_DEBUG
#define LINKED_LIST_BUFFER_SIZE_AWARE
#define LINKED_LIST_BUFFER_DUALLY_LINKED
#define LINKED_LIST_BUFFER_ENTRY_DATA_TYPE unsigned int

#define LINKED_LIST_INLINE_IMPLEMENTATION
#define LINKED_LIST_STATIC_IMPLEMENTATION
#define LINKED_LIST_ASSERT(...) assert(__VA_ARGS__)

//following just for testing
#define LINKED_LIST_ONLY_PROTOTYPE_DECLARATION
#include "linkedListBuffer.h" //Prototype
#undef LINKED_LIST_ONLY_PROTOTYPE_DECLARATION
#include "linkedListBuffer.h" //Implementation
/***********************************************************************************/

static linkedListBufferEntry_t bufferPool[10000];
static linkedListBuffer_t s_linkedListBuffer = 
{
    .bufferPool = bufferPool,
    .bufferPoolSize = sizeof(bufferPool)/sizeof(bufferPool[0]),
    .root = NULL,
#ifdef LINKED_LIST_BUFFER_SIZE_AWARE
    .numActiveEntries = 0,
#endif
};

int main(void)
{
    clock_t startTime = clock();
    linkedList_reset(&s_linkedListBuffer);

    for (size_t r = 0; r < 10; r++)
    {
        linkedListBufferEntry_t * entry = linkedList_allocEntry(&s_linkedListBuffer);
        entry->data = 0;

        printf("Round %i\n",r+1);
        assert(linkedList_getLast(&s_linkedListBuffer) == entry);//last of linked list
        assert(linkedList_getFirst(&s_linkedListBuffer) == entry);//first of linked List
        assert(entry->next == entry);//marked als last of linked list
        assert(linkedList_getNumActiveBuffer(&s_linkedListBuffer) == 1);

        for (size_t i = 1; i < s_linkedListBuffer.bufferPoolSize; i++)
        {
            linkedListBufferEntry_t * entry = linkedList_allocEntry(&s_linkedListBuffer);
            entry->data = i;

            assert(linkedList_getLast(&s_linkedListBuffer) == entry);//last of linked list
            assert(entry->next == entry);//marked als lasnum linkedList bufferPool: %i \nt of linked list
            assert(linkedList_getNumActiveBuffer(&s_linkedListBuffer) == (i + 1));
            assert(linkedList_getNumBufferLeft(&s_linkedListBuffer) == ( s_linkedListBuffer.bufferPoolSize - (i + 1) ) );
        }
        // printf("num active entries: %i \n", linkedList_getNumActiveBuffer(&s_linkedListBuffer));

        entry = linkedList_getFirst(&s_linkedListBuffer);
        for (size_t i = 0; i < ((( s_linkedListBuffer.bufferPoolSize / 2 )-1)); i++)
        {
            entry = linkedList_getNext(&s_linkedListBuffer, entry);
            entry = linkedList_getNext(&s_linkedListBuffer, entry);
            linkedList_freeEntry(&s_linkedListBuffer, linkedList_getPrevious(&s_linkedListBuffer, entry));
            // printf("num active entries: %i \n", linkedList_getNumActiveBuffer(&s_linkedListBuffer));
        }

        //kill the root 
        linkedList_freeEntry(&s_linkedListBuffer, linkedList_getFirst(&s_linkedListBuffer));

        for (size_t i = 0; i < ((( s_linkedListBuffer.bufferPoolSize / 2 )-1)); i++)
        {
            linkedListBufferEntry_t * entry = linkedList_allocEntry(&s_linkedListBuffer);
            entry->data = i + 1000;

            assert(linkedList_getLast(&s_linkedListBuffer) == entry);//last of linked list
            assert(entry->next == entry);//marked als last of linked list
        }

        entry = linkedList_getFirst(&s_linkedListBuffer);
        while (entry != NULL)
        {
            // printf("\n Reading: %i \n", entry->data);
            linkedList_freeEntry(&s_linkedListBuffer, entry);
            // printf("num Entries Left: %i \n", linkedList_getNumActiveBuffer(&s_linkedListBuffer));
            entry = linkedList_getFirst(&s_linkedListBuffer);
        }
    }
    double elapsedTime = (double)(clock() - startTime) / CLOCKS_PER_SEC;
    printf("Done in %f seconds\n", elapsedTime);
}