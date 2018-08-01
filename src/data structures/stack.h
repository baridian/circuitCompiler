//
// Created by ian on 7/31/2018.
//

#ifndef CRCT_STACK_H
#define CRCT_STACK_H

#endif //CRCT_STACK_H

static const START_SIZE = 4;

typedef struct stack
{
    void *stackBase;
    void *stackP;
    void *temp;
    int allocated;
    unsigned dataSize;
} stack;

stack newStack(int);

static void rescaleStack(stack *);

void *spop(stack *);

void spush(void *, stack *);

void *speek(stack);

void frees(stack);