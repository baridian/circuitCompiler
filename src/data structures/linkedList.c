#include <malloc.h>
#include <mem.h>
#include <stdio.h>
#include <assert.h>
#include "linkedList.h"

int llLength(linkedList list)
{
	int i = 0;
	linkedListNode *currentNode = list.head;
	if(currentNode)
		for (i = 1; currentNode = currentNode->next; i++);
	return i;
}

static void negativeIndexWrap(linkedList list, int *index)
{
	*index = *index >= 0 ? *index : (llLength(list) > 0 ? llLength(list) + *index : 0);
}

void llInsert(linkedList *list, int index, void *data)
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
				fprintf(stderr, "ERROR: linked list out of bounds exception\n");
				exit(1);
			}
		}
		temp = currentNode->next;
		currentNode->next = (linkedListNode *) malloc(sizeof(linkedListNode));
		currentNode->next->next = temp;
		currentNode->next->data = malloc(list->dataSize);
		memcpy(currentNode->next->data, data, list->dataSize);
	}
	else
	{
		temp = list->head;
		list->head = (linkedListNode *) malloc(sizeof(linkedListNode));
		list->head->next = temp;
		list->head->data = malloc(list->dataSize);
		memcpy(list->head->data, data, list->dataSize);
	}
}

void llErase(linkedList *list, int index)
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
				fprintf(stderr, "ERROR: linked list out of bounds exception\n");
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
		temp = list->head->next;
		free(list->head->data);
		free(list->head);
		list->head = temp;
	}
}

void *llread(linkedList list, int index)
{
	int i;
	linkedListNode *currentNode = list.head;
	negativeIndexWrap(list, &index);
	for (i = 0; i < index; i++)
		currentNode = currentNode->next;
	return currentNode->data;
}

void llAppend(linkedList *list, void *data)
{
	linkedListNode *currentNode = list->head;
	if(currentNode)
	{
		while (currentNode->next != NULL)
			currentNode = currentNode->next;
		currentNode->next = (linkedListNode *) malloc(sizeof(linkedListNode));
		currentNode->next->next = NULL;
		currentNode->next->data = malloc(list->dataSize);
		memcpy(currentNode->next->data, data, list->dataSize);
	}
	else
	{
		list->head = (linkedListNode *) malloc(sizeof(linkedListNode));
		list->head->next = NULL;
		list->head->data = malloc(list->dataSize);
		memcpy(list->head->data,data,list->dataSize);
	}
}

void llInsertList(linkedList *list, int index, linkedList toInsert)
{
	int i;
	linkedListNode *currentNode = list->head;
	linkedListNode *lastNode = toInsert.head;
	linkedListNode *temp;
	while (lastNode->next != NULL)
	{
		lastNode = lastNode->next;
	}
	negativeIndexWrap(*list, &index);
	if (list->dataSize != toInsert.dataSize)
	{
		fprintf(stderr, "ERROR: lists type mismatch\n");
		exit(1);
	}
	if (index != 0)
	{
		for (i = 1; i < index; i++)
		{
			if (currentNode != NULL)
			{
				currentNode = currentNode->next;
			}
			else
			{
				fprintf(stderr, "ERROR: linked list out of bounds exception\n");
				exit(1);
			}
		}
		temp = currentNode->next->next;
		currentNode->next = toInsert.head;
		lastNode->next = temp;
	}
	else
	{
		temp = list->head;
		list->head = toInsert.head;
		lastNode->next = temp;
	}
}

static int memcmpr(void *a, void *b, int length)
{
	int i;
	if(a == NULL)
	{
		if(b == NULL)
			return 1;
		else
			return 0;
	}
	for (i = 0; i < length; i++)
	{
		if (*((char *) a) == *((char *) b))
		{
			(char *) a++;
			(char *) b++;
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
	linkedListNode *recallTo;
	int noMatch;
	int i = 0;
	while(currentNode != NULL) /*move through whole linked list*/
	{
		if(memcmpr(currentNode->data,targetComparisonNode->data,within.dataSize)); /*if first symbols match*/
		{
			recallTo = currentNode; /*set recall point*/
			noMatch = 0; /*set flag*/
			while(currentNode != NULL && targetComparisonNode != NULL) /*while match data and within not exhausted*/
			{
				if(memcmpr(currentNode->data,targetComparisonNode->data,within.dataSize)) /*if next symbols match continue*/
				{
					currentNode = currentNode->next;
					targetComparisonNode = targetComparisonNode->next;
				}
				else /*if they don't match set flag and exit*/
				{
					noMatch = 1;
					break;
				}
				if(currentNode == NULL && targetComparisonNode != NULL) /*if within exhausted but not at end of match data*/
				{
					noMatch = 1; /*set flag and exit*/
					break;
				}
			}
			currentNode = recallTo; /*return to recall point*/
			if(!noMatch) /*if flag not set return index*/
				return i;
		}
		currentNode = currentNode->next; /*move to next symbol in within*/
		i++;/*increment index*/
	}
	return -1; /*return -1 if not found*/
}

linkedList arrayToll(void *array, int dataSize, int length)
{
	linkedList toReturn;
	int i;
	toReturn.head = NULL;
	toReturn.dataSize = (unsigned) dataSize;
	for (i = 0; i < length; i++)
		llAppend(&toReturn, (char *) array + i * dataSize);
	return toReturn;
}

void llToArray(linkedList list, void *array)
{
	int i;
	for(i=0;i<llLength(list);i++)
		memcpy((char *)array + list.dataSize * i, llread(list,i),list.dataSize);
}

static void freeHelper(linkedListNode *start)
{
	if (start->next != NULL)
		freeHelper(start->next);
	free(start->data);
	free(start);
}

void freell(linkedList list)
{
	if(list.head != NULL)
		freeHelper(list.head);
	list.head = NULL;
}