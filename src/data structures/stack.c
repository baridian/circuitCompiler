//
// Created by ian on 7/31/2018.
//

#include <malloc.h>
#include <string.h>
#include "stack.h"

stack newStack(int dataSize)
{
    stack toReturn;
    toReturn.dataSize = (unsigned)dataSize;
    toReturn.allocated = dataSize * START_SIZE;
    toReturn.stackBase = malloc(toReturn.dataSize * START_SIZE);
    toReturn.temp = malloc(toReturn.dataSize);
    toReturn.stackP =  toReturn.stackBase;
    return toReturn;
}

void *spop(stack *s)
{
    if(s->stackBase == s->stackP)
        return NULL;
    else
    {
        s->stackP = (char *)s->stackP - s->dataSize;
        memcpy(s->temp,s->stackP,s->dataSize);

    }
}

void spush(void *, stack);

void *speek(stack);

void frees(stack);