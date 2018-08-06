#include <malloc.h>
#include <string.h>
#include "stack.h"

stack newStack(int dataSize)
{
	stack toReturn;
	toReturn.dataSize = (unsigned) dataSize;
	toReturn.allocated = START_SIZE;
	toReturn.stackBase = malloc(toReturn.dataSize * START_SIZE);
	toReturn.temp = malloc(toReturn.dataSize);
	toReturn.stackP = toReturn.stackBase;
	return toReturn;
}

static void rescaleStack(stack *s)
{
	int stackSize = ((int) s->stackP - (int) s->stackBase) / s->dataSize;
	if ((float) stackSize < (float) s->allocated * 0.35 && s->allocated > 4)
		s->allocated /= 2;
	else if ((float) stackSize > (float) s->allocated * 0.7)
		s->allocated *= 2;
	else
		return;
	s->stackBase = realloc(s->stackBase, s->allocated * s->dataSize);
	s->stackP = (char *) s->stackBase + stackSize * s->dataSize;
}

void *spop(stack *s)
{
	if (s->stackBase == s->stackP)
		return NULL;
	else
	{
		s->stackP = (char *) s->stackP - s->dataSize;
		memcpy(s->temp, s->stackP, s->dataSize);
		rescaleStack(s);
	}
	return s->temp;
}

void spush(void *data, stack *s)
{
	memcpy(s->stackP, data, s->dataSize);
	s->stackP = (char *) s->stackP + s->dataSize;
	rescaleStack(s);
}

void *speek(stack s)
{
	memcpy(s.temp, (char *)s.stackP - s.dataSize, s.dataSize);
	return s.temp;
}

void frees(stack s)
{
	free(s.stackBase);
	free(s.temp);
}

int stackSize(stack s)
{
	return ((unsigned) s.stackP - (unsigned) s.stackBase) / s.dataSize;
}