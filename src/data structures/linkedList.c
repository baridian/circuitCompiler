#include <malloc.h>
#include <mem.h>
#include <stdio.h>
#include <assert.h>
#include "linkedList.h"

int llLength(linkedList list)
{
    int i;
    linkedListNode *currentNode = list.head;
    for(i = 0;currentNode = currentNode->next;i++);
    return i;
}

static void negativeIndexWrap(linkedList list, int *index)
{
    *index = *index < 0 ? *index : llLength(list) - *index;
}

void llInsert(linkedList *list, int index, void *data)
{
    int i;
    linkedListNode *currentNode = list->head;
    linkedListNode *temp;
    negativeIndexWrap(*list,&index);
    if(index != 0)
    {
        for(i = 1;i < index;i++)
        {
            if(currentNode != NULL)
                currentNode = currentNode->next;
            else
            {
                fprintf(stderr,"ERROR: linked list out of bounds exception");
                exit(1);
            }
        }
        temp = currentNode->next;
        currentNode->next = (linkedListNode *)malloc(sizeof(linkedListNode));
        currentNode->next->next = temp;
        currentNode->next->data = malloc(list->dataSize);
        memcpy(currentNode->next->data,data,list->dataSize);
    }
    else
    {
        temp = list->head;
        list->head = (linkedListNode *)malloc(sizeof(linkedListNode));
        list->head->next = temp;
        list->head->data = malloc(list->dataSize);
        memcpy(list->head->data,data,list->dataSize);
    }
}

void llErase(linkedList *list,int index)
{
    int i;
    linkedListNode *currentNode = list->head;
    linkedListNode *temp;
    negativeIndexWrap(*list, &index);
    if (index != 0)
    {
        for (i = 1; i < index; i++)
        {
            if (currentNode != NULL)
                currentNode = currentNode->next;
            else
            {
                fprintf(stderr, "ERROR: linked list out of bounds exception");
                exit(1);
            }
        }
        temp = currentNode->next->next;
        free(currentNode->next->data);
        free(currentNode->next);
        currentNode->next = temp;
    }
    else
    {
        temp=list->head->next;
        free(list->head->data);
        free(list->head);
        list->head = temp;
    }
}

void *llread(linkedList list, int index)
{
    int i;
    linkedListNode *currentNode = list.head;
    negativeIndexWrap(list,&index);
    for(i=0;i<index;i++)
        currentNode = currentNode->next;
    return currentNode->data;
}

void llAppend(linkedList *list, void *data)
{
    linkedListNode *currentNode = list->head;
    while(currentNode->next != NULL)
        currentNode = currentNode->next;
    currentNode->next = (linkedListNode *)malloc(sizeof(linkedListNode));
    currentNode->next->next = NULL;
    currentNode->next->data = malloc(list->dataSize);
    memcpy(currentNode->next->data,data,list->dataSize);
}

void llInsertList(linkedList *list, int index, linkedList toInsert)
{
    int i;
    linkedListNode *currentNode = list->head;
    linkedListNode *lastNode = toInsert.head;
    linkedListNode *temp;
    currentNode->next = toInsert.head;
    while (lastNode->next != NULL)
        lastNode = lastNode->next;
    negativeIndexWrap(*list, &index);
    if(list->dataSize != toInsert.dataSize)
    {
        fprintf(stderr,"ERROR: lists type mismatch");
        exit(1);
    }
    if (index != 0)
    {
        for (i = 1; i < index; i++)
        {
            if (currentNode != NULL)
                currentNode = currentNode->next;
            else
            {
                fprintf(stderr, "ERROR: linked list out of bounds exception");
                exit(1);
            }
        }
        temp = currentNode->next->next;
        lastNode->next = temp;
    }
    else
    {
        temp = list->head;
        list->head = toInsert.head;
        lastNode->next = temp;
    }
}

static int voidcmpr(void *a, void *b, int length)
{
    int i;
    for(i = 0; i < length; i++)
    {
        if(*((char *)a) == *((char *)b))
        {
            (char *)a++;
            (char *)b++;
        }
        else
            return 0;
    }
    return 1;
}


int llMatch(linkedList within, linkedList target)
{
    linkedListNode *currentNode = within.head;
    linkedListNode *targetComparisonNode = target.head;
    int i = 0;
    if(within.dataSize != target.dataSize)
        return 0;
    while(currentNode != NULL)
    {
        if(voidcmpr(currentNode->data,targetComparisonNode->data, within.dataSize))
        {
            return i;
        }
        else
        {
            currentNode = currentNode->next;
            targetComparisonNode = targetComparisonNode->next;
            i++;
        }
    }
    return -1;
}

linkedList arrayToll(void *array, int dataSize, int length)
{
    linkedList toReturn;
    int i;
    toReturn.dataSize = (unsigned)dataSize;
    for(i=0;i<length;i++)
        llInsert(&toReturn,-1,(char *)array + length * dataSize);
    return toReturn;
}

static void freeHelper(linkedListNode *start)
{
    if(start->next != NULL)
        freeHelper(start->next);
    free(start->data);
    free(start);
}

void freell(linkedList list)
{
    freeHelper(list.head);
}