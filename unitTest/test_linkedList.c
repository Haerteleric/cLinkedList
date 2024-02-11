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
//#define LINKED_LIST_INLINE_IMPLEMENTATION
#define LINKED_LIST_STATIC_IMPLEMENTATION
#define LINKED_LIST_ASSERT(...) assert(__VA_ARGS__)

#include "linkedListBuffer_t.h" 
/***********************************************************************************/

static linkedListBufferEntry_t entryPool[10000];
static linkedListBuffer_t s_linkedListBuffer = 
{
    .entryPool = entryPool,
    .entryPoolSize = sizeof(entryPool)/sizeof(entryPool[0]),
    .rootEntry = NULL,
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
void test_rejectWhenFull(void)
{
    linkedListBufferEntry_t * entry = NULL;

    entry = linkedList_allocEntryAtEnd(&s_linkedListBuffer);
    entry->data = 0;

    TEST_ASSERT_EQUAL(linkedList_getFirstEntry(&s_linkedListBuffer), entry); //entry should be first in Linked list
    TEST_ASSERT_EQUAL(linkedList_getLastEntry(&s_linkedListBuffer), entry); //entry should be last in Linked list

    TEST_ASSERT_EQUAL(entry->nextEntry, entry); //entry should be marked as  Linked list End

#ifdef LINKED_LIST_BUFFER_DUALLY_LINKED
    TEST_ASSERT_EQUAL(entry->previousEntry, entry); //entry should be marked as Linked list Begin
#endif

    TEST_ASSERT_EQUAL(linkedList_getNumEntriesUsed(&s_linkedListBuffer), 1);

    /* Fill Linked List */
    for (size_t i = 1; i < s_linkedListBuffer.entryPoolSize; i++)
    {
        //Alloc Entry
        linkedListBufferEntry_t * entry = linkedList_allocEntryAtEnd(&s_linkedListBuffer);
        //Fill Entry
        entry->data = i;

        //Check Memory Stats
        TEST_ASSERT_EQUAL_PTR(linkedList_getLastEntry(&s_linkedListBuffer), entry); //entry should be last in Linked list
        TEST_ASSERT_EQUAL_PTR(entry->nextEntry, entry); //entry should be marked as  Linked list End
        TEST_ASSERT_EQUAL(linkedList_getNumEntriesUsed(&s_linkedListBuffer), i+1);
        TEST_ASSERT_EQUAL(linkedList_getNumEntriesFree(&s_linkedListBuffer), ( s_linkedListBuffer.entryPoolSize - (i + 1) ) );
    }

    TEST_ASSERT_EQUAL_PTR(NULL, linkedList_allocEntryAtEnd(&s_linkedListBuffer));
    TEST_ASSERT_EQUAL_PTR(NULL, linkedList_allocEntryAtBegin(&s_linkedListBuffer));
}

void test_pushAndPop(void)
{
    linkedListBufferEntry_t * entry = NULL;

    entry = linkedList_allocEntryAtEnd(&s_linkedListBuffer);
    entry->data = 0;

    TEST_ASSERT_EQUAL(linkedList_getFirstEntry(&s_linkedListBuffer), entry); //entry should be first in Linked list
    TEST_ASSERT_EQUAL(linkedList_getLastEntry(&s_linkedListBuffer), entry); //entry should be last in Linked list

    TEST_ASSERT_EQUAL(entry->nextEntry, entry); //entry should be marked as  Linked list End

#ifdef LINKED_LIST_BUFFER_DUALLY_LINKED
    TEST_ASSERT_EQUAL(entry->previousEntry, entry); //entry should be marked as Linked list Begin
#endif

   TEST_ASSERT_EQUAL(linkedList_getNumEntriesUsed(&s_linkedListBuffer), 1);

    /* Fill Linked List */
    for (size_t i = 1; i < s_linkedListBuffer.entryPoolSize; i++)
    {
        //Alloc Entry
        linkedListBufferEntry_t * entry = linkedList_allocEntryAtEnd(&s_linkedListBuffer);
        //Fill Entry
        entry->data = i;

        //Check Memory Stats
        TEST_ASSERT_EQUAL_PTR(linkedList_getLastEntry(&s_linkedListBuffer), entry); //entry should be last in Linked list
        TEST_ASSERT_EQUAL_PTR(entry->nextEntry, entry); //entry should be marked as  Linked list End
        TEST_ASSERT_EQUAL(linkedList_getNumEntriesUsed(&s_linkedListBuffer), i+1);
        TEST_ASSERT_EQUAL(linkedList_getNumEntriesFree(&s_linkedListBuffer), ( s_linkedListBuffer.entryPoolSize - (i + 1) ) );
    }

    entry = NULL;

    /* Flush Linked List */
    for (size_t i = 0; i < s_linkedListBuffer.entryPoolSize; i++)
    {
        //Get Entry
        entry = linkedList_getFirstEntry(&s_linkedListBuffer);

        //Inspect Entry
        TEST_ASSERT_NOT_NULL( entry);
        TEST_ASSERT_EQUAL(entry->data, i);

        //Delete Entry
        TEST_ASSERT_TRUE(linkedList_freeEntry(&s_linkedListBuffer,entry));
        TEST_ASSERT_EQUAL(linkedList_getNumEntriesFree(&s_linkedListBuffer), i+1);
    }

    //Confirm List is empty
    TEST_ASSERT_EQUAL(0, linkedList_getNumEntriesUsed(&s_linkedListBuffer));
    
    //Confirm no entries can be retrieved
    TEST_ASSERT_EQUAL_PTR(NULL, linkedList_getFirstEntry(&s_linkedListBuffer));
    TEST_ASSERT_EQUAL_PTR(NULL, linkedList_getLastEntry(&s_linkedListBuffer));
}

void test_pushAndPopReversed(void)
{
    linkedListBufferEntry_t * entry = NULL;

    entry = linkedList_allocEntryAtBegin(&s_linkedListBuffer);
    entry->data = 0;

    TEST_ASSERT_EQUAL_PTR(linkedList_getFirstEntry(&s_linkedListBuffer), entry); //entry should be first in Linked list
    TEST_ASSERT_EQUAL_PTR(linkedList_getLastEntry(&s_linkedListBuffer), entry); //entry should be last in Linked list

    TEST_ASSERT_EQUAL_PTR(entry->nextEntry, entry); //entry should be marked as  Linked list End

#ifdef LINKED_LIST_BUFFER_DUALLY_LINKED
    TEST_ASSERT_EQUAL(entry->previousEntry, entry); //entry should be marked as Linked list Begin
#endif

   TEST_ASSERT_EQUAL(1, linkedList_getNumEntriesUsed(&s_linkedListBuffer));

    /* Fill Linked List */
    for (size_t i = 1; i < s_linkedListBuffer.entryPoolSize; i++)
    {
        //Alloc Entry
        linkedListBufferEntry_t * entry = linkedList_allocEntryAtBegin(&s_linkedListBuffer);
        //Fill Entry
        entry->data = i;

        //Check Memory Stats
        TEST_ASSERT_EQUAL(linkedList_getFirstEntry(&s_linkedListBuffer), entry); //entry should be first in Linked list

#ifdef LINKED_LIST_BUFFER_DUALLY_LINKED
    TEST_ASSERT_EQUAL(entry->previousEntry, entry); //entry should be marked as Linked list Begin
#endif

        TEST_ASSERT_EQUAL(linkedList_getNumEntriesUsed(&s_linkedListBuffer), i+1);
        TEST_ASSERT_EQUAL(linkedList_getNumEntriesFree(&s_linkedListBuffer), ( s_linkedListBuffer.entryPoolSize - (i + 1) ) );
    }

    entry = NULL;

    /* Flush Linked List */
    for (size_t i = 0; i < s_linkedListBuffer.entryPoolSize; i++)
    {
        //Get Entry
        entry = linkedList_getLastEntry(&s_linkedListBuffer);

        //Inspect Entry
        TEST_ASSERT_NOT_NULL( entry);
        TEST_ASSERT_EQUAL(entry->data, i);

        //Delete Entry
        TEST_ASSERT_TRUE(linkedList_freeEntry(&s_linkedListBuffer,entry));
        TEST_ASSERT_EQUAL(linkedList_getNumEntriesFree(&s_linkedListBuffer), i+1);
    }

    //Confirm List is empty
    TEST_ASSERT_EQUAL(0, linkedList_getNumEntriesUsed(&s_linkedListBuffer));

    //Confirm no entries can be retrieved
    TEST_ASSERT_EQUAL_PTR(NULL, linkedList_getFirstEntry(&s_linkedListBuffer));
    TEST_ASSERT_EQUAL_PTR(NULL, linkedList_getLastEntry(&s_linkedListBuffer));
}

void test_removeEntriesAndInsertIntoNonEmpty(void)
{
    linkedListBufferEntry_t * entry = NULL;

    entry = linkedList_allocEntryAtEnd(&s_linkedListBuffer);
    entry->data = 0;

    TEST_ASSERT_EQUAL_PTR(linkedList_getFirstEntry(&s_linkedListBuffer), entry); //entry should be first in Linked list
    TEST_ASSERT_EQUAL_PTR(linkedList_getLastEntry(&s_linkedListBuffer), entry); //entry should be last in Linked list

    TEST_ASSERT_EQUAL_PTR(entry->nextEntry, entry); //entry should be marked as  Linked list End

#ifdef LINKED_LIST_BUFFER_DUALLY_LINKED
    TEST_ASSERT_EQUAL(entry->previousEntry, entry); //entry should be marked as Linked list Begin
#endif

   TEST_ASSERT_EQUAL(linkedList_getNumEntriesUsed(&s_linkedListBuffer), 1);

    for (size_t i = 1; i < s_linkedListBuffer.entryPoolSize; i++)
    {
        //Alloc Entry
        linkedListBufferEntry_t * entry = linkedList_allocEntryAtEnd(&s_linkedListBuffer);
        //Fill Entry
        entry->data = i;

        //Check Memory Stats
        TEST_ASSERT_EQUAL(linkedList_getLastEntry(&s_linkedListBuffer), entry); //entry should be last in Linked list
        TEST_ASSERT_EQUAL(entry->nextEntry, entry); //entry should be marked as  Linked list End
        TEST_ASSERT_EQUAL(i+1, linkedList_getNumEntriesUsed(&s_linkedListBuffer));
        TEST_ASSERT_EQUAL(( s_linkedListBuffer.entryPoolSize - (i + 1) ), linkedList_getNumEntriesFree(&s_linkedListBuffer)) ;
    }

    /* Remove every Second Entry */
    unsigned int removedEntries = 0;
    unsigned int remainingEntries = linkedList_getNumEntriesUsed(&s_linkedListBuffer);

    entry = linkedList_getFirstEntry(&s_linkedListBuffer);

    for (size_t i = 0; i < s_linkedListBuffer.entryPoolSize; i++)
    {
        TEST_ASSERT_NOT_NULL( entry);

        linkedListBufferEntry_t * nextEntry = linkedList_getNext(&s_linkedListBuffer,entry);

        if(i < (s_linkedListBuffer.entryPoolSize-1))
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
    TEST_ASSERT_EQUAL(remainingEntries, linkedList_getNumEntriesUsed(&s_linkedListBuffer));
    TEST_ASSERT_EQUAL(removedEntries, linkedList_getNumEntriesFree(&s_linkedListBuffer));

    /* Fill the now empty Buffer Slots*/
    for (size_t i = 0; i < removedEntries; i++)
    {
        //Alloc Entry
        linkedListBufferEntry_t * entry = linkedList_allocEntryAtEnd(&s_linkedListBuffer);
        
        //Fill Entry (with offset)
        entry->data = i + (s_linkedListBuffer.entryPoolSize * 2);
    }

    TEST_ASSERT_EQUAL(s_linkedListBuffer.entryPoolSize, linkedList_getNumEntriesUsed(&s_linkedListBuffer));
    TEST_ASSERT_EQUAL(0, linkedList_getNumEntriesFree(&s_linkedListBuffer));

    /* Remove the original Entries */
    for (size_t i = 0; i < remainingEntries; i++)
    {
        //Get Entry
        entry = linkedList_getFirstEntry(&s_linkedListBuffer);

        //Inspect Entry
        TEST_ASSERT_NOT_NULL( entry);
        TEST_ASSERT_EQUAL(i*2, entry->data);

        //Delete Entry
        TEST_ASSERT_TRUE(linkedList_freeEntry(&s_linkedListBuffer,entry));
    }
    
    //Check that only Entries form the second round are in the Buffer
    TEST_ASSERT_EQUAL(removedEntries, linkedList_getNumEntriesUsed(&s_linkedListBuffer));

     /* Flush Linked List */
    for (size_t i = 0; i < removedEntries; i++)
    {
        //Get Entry
        entry = linkedList_getFirstEntry(&s_linkedListBuffer);

        //Inspect Entry
        TEST_ASSERT_NOT_NULL( entry);
        TEST_ASSERT_EQUAL(i + (s_linkedListBuffer.entryPoolSize * 2), entry->data);

        //Delete Entry
        TEST_ASSERT_TRUE(linkedList_freeEntry(&s_linkedListBuffer,entry));
    }

    //Confirm List is empty
    TEST_ASSERT_EQUAL(0, linkedList_getNumEntriesUsed(&s_linkedListBuffer));

    //Confirm no entries can be retrieved
    TEST_ASSERT_EQUAL_PTR(NULL, linkedList_getFirstEntry(&s_linkedListBuffer));
    TEST_ASSERT_EQUAL_PTR(NULL, linkedList_getLastEntry(&s_linkedListBuffer));
}

void test_removeEntriesAndInsertIntoNonEmptyReversed(void)
{
    linkedListBufferEntry_t * entry = NULL;

    entry = linkedList_allocEntryAtBegin(&s_linkedListBuffer);
    entry->data = 0;

    TEST_ASSERT_EQUAL_PTR(linkedList_getFirstEntry(&s_linkedListBuffer), entry); //entry should be first in Linked list
    TEST_ASSERT_EQUAL_PTR(linkedList_getLastEntry(&s_linkedListBuffer), entry); //entry should be last in Linked list

    TEST_ASSERT_EQUAL_PTR(entry->nextEntry, entry); //entry should be marked as  Linked list End

#ifdef LINKED_LIST_BUFFER_DUALLY_LINKED
    TEST_ASSERT_EQUAL(entry->previousEntry, entry); //entry should be marked as Linked list Begin
#endif

   TEST_ASSERT_EQUAL(linkedList_getNumEntriesUsed(&s_linkedListBuffer), 1);

    for (size_t i = 1; i < s_linkedListBuffer.entryPoolSize; i++)
    {
        //Alloc Entry
        linkedListBufferEntry_t * entry = linkedList_allocEntryAtBegin(&s_linkedListBuffer);
        //Fill Entry
        entry->data = i;

        //Check Memory Stats
        TEST_ASSERT_EQUAL(linkedList_getFirstEntry(&s_linkedListBuffer), entry); //entry should be last in Linked list

#ifdef LINKED_LIST_BUFFER_DUALLY_LINKED
        TEST_ASSERT_EQUAL(entry->previousEntry, entry); //entry should be marked as Linked list Begin
#endif
        TEST_ASSERT_EQUAL(i+1, linkedList_getNumEntriesUsed(&s_linkedListBuffer));
        TEST_ASSERT_EQUAL(( s_linkedListBuffer.entryPoolSize - (i + 1) ), linkedList_getNumEntriesFree(&s_linkedListBuffer)) ;
    }

    /* Remove every Second Entry */
    unsigned int removedEntries = 0;
    unsigned int remainingEntries = linkedList_getNumEntriesUsed(&s_linkedListBuffer);

    entry = linkedList_getLastEntry(&s_linkedListBuffer);

    for (size_t i = 0; i < s_linkedListBuffer.entryPoolSize; i++)
    {
        TEST_ASSERT_NOT_NULL( entry);

        linkedListBufferEntry_t * nextEntry = linkedList_getPrevious(&s_linkedListBuffer,entry);

        if(i < (s_linkedListBuffer.entryPoolSize-1))
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
    TEST_ASSERT_EQUAL(remainingEntries, linkedList_getNumEntriesUsed(&s_linkedListBuffer));
    TEST_ASSERT_EQUAL(removedEntries, linkedList_getNumEntriesFree(&s_linkedListBuffer));

    /* Fill the now empty Buffer Slots*/
    for (size_t i = 0; i < removedEntries; i++)
    {
        //Alloc Entry
        linkedListBufferEntry_t * entry = linkedList_allocEntryAtBegin(&s_linkedListBuffer);
        
        //Fill Entry (with offset)
        entry->data = i + (s_linkedListBuffer.entryPoolSize * 2);
    }

    TEST_ASSERT_EQUAL(s_linkedListBuffer.entryPoolSize, linkedList_getNumEntriesUsed(&s_linkedListBuffer));
    TEST_ASSERT_EQUAL(0, linkedList_getNumEntriesFree(&s_linkedListBuffer));

    /* Remove the original Entries */
    for (size_t i = 0; i < remainingEntries; i++)
    {
        //Get Entry
        entry = linkedList_getLastEntry(&s_linkedListBuffer);

        //Inspect Entry
        TEST_ASSERT_NOT_NULL( entry);
        TEST_ASSERT_EQUAL(i*2, entry->data);

        //Delete Entry
        TEST_ASSERT_TRUE(linkedList_freeEntry(&s_linkedListBuffer,entry));
    }
    
    //Check that only Entries form the second round are in the Buffer
    TEST_ASSERT_EQUAL(removedEntries, linkedList_getNumEntriesUsed(&s_linkedListBuffer));

     /* Flush Linked List */
    for (size_t i = 0; i < removedEntries; i++)
    {
        //Get Entry
        entry = linkedList_getLastEntry(&s_linkedListBuffer);

        //Inspect Entry
        TEST_ASSERT_NOT_NULL( entry);
        TEST_ASSERT_EQUAL(i + (s_linkedListBuffer.entryPoolSize * 2), entry->data);

        //Delete Entry
        TEST_ASSERT_TRUE(linkedList_freeEntry(&s_linkedListBuffer,entry));
    }

    //Confirm List is empty
    TEST_ASSERT_EQUAL(0, linkedList_getNumEntriesUsed(&s_linkedListBuffer));

    //Confirm no entries can be retrieved
    TEST_ASSERT_EQUAL_PTR(NULL, linkedList_getFirstEntry(&s_linkedListBuffer));
    TEST_ASSERT_EQUAL_PTR(NULL, linkedList_getLastEntry(&s_linkedListBuffer));
}

void test_allocBefore(void)
{
    unsigned int inputEntries[] = {1,2,3,4,5};
    unsigned int insertedEntry = 77;
    unsigned int expectedOutputEntries[] = {77,1,2,77,3,4,5};

    linkedListBufferEntry_t * targetEntry = NULL;
    for (size_t i = 0; i < sizeof(inputEntries)/sizeof(inputEntries[0]); i++)
    {
        linkedListBufferEntry_t * entry = linkedList_allocEntryAtEnd(&s_linkedListBuffer);
        entry->data = inputEntries[i];

        if(inputEntries[i] == 3)
        {
            targetEntry = entry;
        }
    }

    linkedListBufferEntry_t * entry = linkedList_allocEntryBefore(&s_linkedListBuffer, targetEntry);
    entry->data = insertedEntry;

    entry = linkedList_allocEntryBefore(&s_linkedListBuffer, linkedList_getFirstEntry(&s_linkedListBuffer));
    entry->data = insertedEntry;

    for (size_t i = 0; i < sizeof(expectedOutputEntries)/sizeof(expectedOutputEntries[0]); i++)
    {
        linkedListBufferEntry_t * entry = linkedList_getFirstEntry(&s_linkedListBuffer);

        TEST_ASSERT_EQUAL(expectedOutputEntries[i], entry->data);

        linkedList_freeEntry(&s_linkedListBuffer, entry);
    }

    //Confirm List is empty
    TEST_ASSERT_EQUAL(0, linkedList_getNumEntriesUsed(&s_linkedListBuffer));

    //Confirm no entries can be retrieved
    TEST_ASSERT_EQUAL_PTR(NULL, linkedList_getFirstEntry(&s_linkedListBuffer));
    TEST_ASSERT_EQUAL_PTR(NULL, linkedList_getLastEntry(&s_linkedListBuffer));
}

void test_allocAfter(void)
{
    unsigned int inputEntries[] = {1,2,3,4,5};
    unsigned int insertedEntry = 77;
    unsigned int expectedOutputEntries[] = {1,2,3,77,4,5,77};

    linkedListBufferEntry_t * targetEntry = NULL;
    for (size_t i = 0; i < sizeof(inputEntries)/sizeof(inputEntries[0]); i++)
    {
        linkedListBufferEntry_t * entry = linkedList_allocEntryAtEnd(&s_linkedListBuffer);
        entry->data = inputEntries[i];

        if(inputEntries[i] == 3)
        {
            targetEntry = entry;
        }
    }

    linkedListBufferEntry_t * entry = linkedList_allocEntryAfter(&s_linkedListBuffer, targetEntry);
    entry->data = insertedEntry;

    entry = linkedList_allocEntryAfter(&s_linkedListBuffer, linkedList_getLastEntry(&s_linkedListBuffer));
    entry->data = insertedEntry;


    for (size_t i = 0; i < sizeof(expectedOutputEntries)/sizeof(expectedOutputEntries[0]); i++)
    {
        linkedListBufferEntry_t * entry = linkedList_getFirstEntry(&s_linkedListBuffer);

        TEST_ASSERT_EQUAL(expectedOutputEntries[i], entry->data);

        linkedList_freeEntry(&s_linkedListBuffer, entry);
    }

    //Confirm List is empty
    TEST_ASSERT_EQUAL(0, linkedList_getNumEntriesUsed(&s_linkedListBuffer));

    //Confirm no entries can be retrieved
    TEST_ASSERT_EQUAL_PTR(NULL, linkedList_getFirstEntry(&s_linkedListBuffer));
    TEST_ASSERT_EQUAL_PTR(NULL, linkedList_getLastEntry(&s_linkedListBuffer));
}