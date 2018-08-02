#ifndef CRCT_LINKEDLIST_H
#define CRCT_LINKEDLIST_H

#endif //CRCT_LINKEDLIST_H

typedef struct linkedListNode linkedListNode;

struct linkedListNode
{
	linkedListNode *next;
	void *data;
};

typedef struct linkedList
{
	linkedListNode *head;
	unsigned dataSize;
} linkedList;

int llLength(linkedList);

static void negativeIndexWrap(linkedList, int *);

void llInsert(linkedList *, int index, void *data);

void llErase(linkedList *, int index);

void *llread(linkedList, int index);

void llAppend(linkedList *, void *data);

void llInsertList(linkedList *insertInto, int index, linkedList inserting);

static int memcmpr(void *, void *, int);

int llMatch(linkedList lookingAt, linkedList matchData);

linkedList arrayToll(void *data, int elementSize, int length);

static void freeHelper(linkedListNode *);

void freell(linkedList);