#include <stdio.h>
#include <assert.h>
#include <time.h>

/*****************************TEST FRAMEWORK INCLUDE**************************************/
#include "unity.h"

#include "fff.h"
DEFINE_FFF_GLOBALS;
/***********************************************************************************/



/*****************************TEMPLATE INCLUDE**************************************/
#define LINKED_LIST_BUFFER_ENTRY_DATA_TYPE unsigned int
#define LINKED_LIST_INLINE_IMPLEMENTATION
#define LINKED_LIST_STATIC_IMPLEMENTATION
#define LINKED_LIST_ASSERT(...) assert(__VA_ARGS__)

#include "linkedListBuffer_t.h" 
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

void setUp(void)
{
    linkedList_reset(&s_linkedListBuffer);
}

void tearDown(void)
{
}


/*****************************TESTS**************************/
void test_pushAndPop(void)
{
    linkedListBufferEntry_t * entry = NULL;

    entry = linkedList_allocEntryAtBegin(&s_linkedListBuffer);
    entry->data = 0;

    TEST_ASSERT_EQUAL(linkedList_getFirst(&s_linkedListBuffer), entry); //entry should be first in LInked list
    TEST_ASSERT_EQUAL(linkedList_getLast(&s_linkedListBuffer), entry); //entry should be last in LInked list

    TEST_ASSERT_EQUAL(entry->next, entry); //entry should be marked as  LInked list End

#ifdef LINKED_LIST_BUFFER_DUALLY_LINKED
    TEST_ASSERT_EQUAL(entry->previous, entry); //entry should be marked as LInked list Begin
#endif

   TEST_ASSERT_EQUAL(linkedList_getNumActiveBuffer(&s_linkedListBuffer), 1);

    /* Fill Linked List */
    for (size_t i = 1; i < s_linkedListBuffer.bufferPoolSize; i++)
    {
        //Alloc Entry
        linkedListBufferEntry_t * entry = linkedList_allocEntryAtBegin(&s_linkedListBuffer);
        //Fill Entry
        entry->data = i;

        //Check Memory Stats
        TEST_ASSERT_EQUAL(linkedList_getLast(&s_linkedListBuffer), entry); //entry should be last in LInked list
        TEST_ASSERT_EQUAL(entry->next, entry); //entry should be marked as  LInked list End
        TEST_ASSERT_EQUAL(linkedList_getNumActiveBuffer(&s_linkedListBuffer), i+1);
        TEST_ASSERT_EQUAL(linkedList_getNumBufferLeft(&s_linkedListBuffer), ( s_linkedListBuffer.bufferPoolSize - (i + 1) ) );
    }

    entry = NULL;

    /* Flush Linked List */
    for (size_t i = 0; i < s_linkedListBuffer.bufferPoolSize; i++)
    {
        //Get Entry
        entry = linkedList_getFirst(&s_linkedListBuffer);

        //Inspect Entry
        TEST_ASSERT_NOT_NULL( entry);
        TEST_ASSERT_EQUAL(entry->data, i);

        //Delete Entry
        TEST_ASSERT_TRUE(linkedList_freeEntry(&s_linkedListBuffer,entry));
        TEST_ASSERT_EQUAL(linkedList_getNumBufferLeft(&s_linkedListBuffer), i+1);
    }

    //Confirm List is empty
    TEST_ASSERT_EQUAL(0, linkedList_getNumActiveBuffer(&s_linkedListBuffer));
    TEST_ASSERT_EQUAL(NULL, linkedList_getFirst(&s_linkedListBuffer)); //entry should be first in LInked list
    TEST_ASSERT_EQUAL(NULL, linkedList_getLast(&s_linkedListBuffer)); //entry should be last in LInked list
}


void test_removeEntriesAndInsertInNonEmptyList(void)
{
    linkedListBufferEntry_t * entry = NULL;

    entry = linkedList_allocEntryAtBegin(&s_linkedListBuffer);
    entry->data = 0;

    TEST_ASSERT_EQUAL(linkedList_getFirst(&s_linkedListBuffer), entry); //entry should be first in LInked list
    TEST_ASSERT_EQUAL(linkedList_getLast(&s_linkedListBuffer), entry); //entry should be last in LInked list

    TEST_ASSERT_EQUAL(entry->next, entry); //entry should be marked as  LInked list End

#ifdef LINKED_LIST_BUFFER_DUALLY_LINKED
    TEST_ASSERT_EQUAL(entry->previous, entry); //entry should be marked as LInked list Begin
#endif

   TEST_ASSERT_EQUAL(linkedList_getNumActiveBuffer(&s_linkedListBuffer), 1);

    for (size_t i = 1; i < s_linkedListBuffer.bufferPoolSize; i++)
    {
        //Alloc Entry
        linkedListBufferEntry_t * entry = linkedList_allocEntryAtBegin(&s_linkedListBuffer);
        //Fill Entry
        entry->data = i;

        //Check Memory Stats
        TEST_ASSERT_EQUAL(linkedList_getLast(&s_linkedListBuffer), entry); //entry should be last in LInked list
        TEST_ASSERT_EQUAL(entry->next, entry); //entry should be marked as  LInked list End
        TEST_ASSERT_EQUAL(i+1, linkedList_getNumActiveBuffer(&s_linkedListBuffer));
        TEST_ASSERT_EQUAL(( s_linkedListBuffer.bufferPoolSize - (i + 1) ), linkedList_getNumBufferLeft(&s_linkedListBuffer)) ;
    }

    /* Remove every Second Entry */
    unsigned int removedEntries = 0;
    unsigned int remainingEntries = linkedList_getNumActiveBuffer(&s_linkedListBuffer);

    entry = linkedList_getFirst(&s_linkedListBuffer);

    for (size_t i = 0; i < s_linkedListBuffer.bufferPoolSize; i++)
    {
        TEST_ASSERT_NOT_NULL( entry);

        linkedListBufferEntry_t * nextEntry = linkedList_getNext(&s_linkedListBuffer,entry);

        if(i < (s_linkedListBuffer.bufferPoolSize-1))
            TEST_ASSERT_EQUAL(entry->data+1, nextEntry->data);

        if((i%2) > 0)
        {
            //Delete Entry
            TEST_ASSERT_TRUE(linkedList_freeEntry(&s_linkedListBuffer,entry));
            removedEntries++;
            remainingEntries--;
        }

        entry = nextEntry;
    }

    //Check Memory Stats
    TEST_ASSERT_EQUAL(remainingEntries, linkedList_getNumActiveBuffer(&s_linkedListBuffer));
    TEST_ASSERT_EQUAL(removedEntries, linkedList_getNumBufferLeft(&s_linkedListBuffer));

    /* Fill the now empty Buffer Slots*/
    for (size_t i = 0; i < removedEntries; i++)
    {
        //Alloc Entry
        linkedListBufferEntry_t * entry = linkedList_allocEntryAtBegin(&s_linkedListBuffer);
        
        //Fill Entry (with offset)
        entry->data = i + (s_linkedListBuffer.bufferPoolSize * 2);
    }

    TEST_ASSERT_EQUAL(s_linkedListBuffer.bufferPoolSize, linkedList_getNumActiveBuffer(&s_linkedListBuffer));
    TEST_ASSERT_EQUAL(0, linkedList_getNumBufferLeft(&s_linkedListBuffer));

    /* Remove the original Entries */
    for (size_t i = 0; i < remainingEntries; i++)
    {
        //Get Entry
        entry = linkedList_getFirst(&s_linkedListBuffer);

        //Inspect Entry
        TEST_ASSERT_NOT_NULL( entry);
        TEST_ASSERT_EQUAL(i*2, entry->data);

        //Delete Entry
        TEST_ASSERT_TRUE(linkedList_freeEntry(&s_linkedListBuffer,entry));
    }
    
    //Check that only Entries form the second round are in the Buffer
    TEST_ASSERT_EQUAL(removedEntries, linkedList_getNumActiveBuffer(&s_linkedListBuffer));

     /* Flush Linked List */
    for (size_t i = 0; i < removedEntries; i++)
    {
        //Get Entry
        entry = linkedList_getFirst(&s_linkedListBuffer);

        //Inspect Entry
        TEST_ASSERT_NOT_NULL( entry);
        TEST_ASSERT_EQUAL(i + (s_linkedListBuffer.bufferPoolSize * 2), entry->data);

        //Delete Entry
        TEST_ASSERT_TRUE(linkedList_freeEntry(&s_linkedListBuffer,entry));
    }

    //Confirm List is empty
    TEST_ASSERT_EQUAL(0, linkedList_getNumActiveBuffer(&s_linkedListBuffer));
    TEST_ASSERT_EQUAL(NULL, linkedList_getFirst(&s_linkedListBuffer)); //entry should be first in LInked list
    TEST_ASSERT_EQUAL(NULL, linkedList_getLast(&s_linkedListBuffer)); //entry should be last in LInked list
}