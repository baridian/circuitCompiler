#ifndef CRCT_STACK_H
#define CRCT_STACK_H

static const int START_SIZE = 4;

typedef struct stack
{
	void *stackBase;
	void *stackP;
	void *temp;
	int allocated;
	unsigned dataSize;
} stack;

stack newStack(int elementSize);

static void rescaleStack(stack *);

void *spop(stack *);

void spush(void *data, stack *);

void *speek(stack);

void frees(stack);

int stackSize(stack);

#endif //CRCT_STACK_H
