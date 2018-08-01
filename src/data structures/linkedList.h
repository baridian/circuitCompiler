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

void llInsert(linkedList *, int, void *);

void llErase(linkedList *,int);

void *llread(linkedList, int);

void llAppend(linkedList *, void *);

void llInsertList(linkedList *, int, linkedList);

static int memcmpr(void *,void *, int);

int llMatch(linkedList, linkedList);

linkedList arrayToll(void *,int , int );

static void freeHelper(linkedListNode *);

void freell(linkedList);