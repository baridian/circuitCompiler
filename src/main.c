#include <stdio.h>
#include <string.h>
#include "data structures/linkedList.h"
#include "data structures/tree.h"

typedef enum symbolType{literal, variable, operator} symbolType;

typedef struct symbol
{
	symbolType type;
	union data
	{
		int literal;
		char *variable;
		char operand[3];
	} data;
} symbol;

int isOperator(char a, char b, char *opTable[], int tableLength)
{
	int i,j;
	for(i=0;i<tableLength;i++)
	{
		for(j=0;opTable[i][j];j++)
		{
			if(opTable[i][j] == ',')
				continue;
			if(a == opTable[i][j])
			{
				if(opTable[i][j+1] != ',' && opTable[i][j+1] != '\0')
				{
					if(b == opTable[i][j+1])
						return 1;
				}
				else
					return 1;
			}
		}
	}
	return 0;
}

int isDoubleCharOperator(char a, char b, char *opTable[], int tableLength)
{
	int i, j;
	for (i = 0; i < tableLength; i++)
	{
		for (j = 0; opTable[i][j]; j++)
		{
			if (opTable[i][j] == ',')
				continue;
			if (a == opTable[i][j])
			{
				if (opTable[i][j + 1] != ',' && opTable[i][j + 1] != '\0')
				{
					if (b == opTable[i][j + 1])
						return 1;
				}
			}
		}
	}
	return 0;
}

linkedList generateSymbolicString(linkedList input, char *opTable[], int tableLength)
{
	int i;
	char a,b;
	symbol start;
	linkedList toReturn;
	int initialized = 0;
	for(i=0;i<llLength(input);i++)
	{
		a = *(char *)llread(input,i);
		if(i != llLength(input) - 1)
			b = *(char *)llread(input,i+1);
		else
			b = '\0';
		if(isOperator(a,b,opTable,tableLength))
		{
			start.type = operator;
			start.data.operand[0] = a;
			if(isDoubleCharOperator(a,b,opTable,tableLength))
				start.data.operand[1] = b;
			else
				start.data.operand[1] = '\0';
			start.data.operand[2] = '\0';
			if(initialized)
				llAppend(&toReturn,&start);
			else
			{
				toReturn = arrayToll(&start,sizeof(symbol),1);
				initialized = 1;
			}
		}
	} /*TODO: add literal and variable detection*/
}

void convertExpression(char *input, char *output,char *opTable[], int tableLength)
{
	linkedList charString = arrayToll(input,sizeof(char),sizeof(input));
	linkedList symbolicString = generateSymbolicString(charString,opTable,tableLength);
	tree expressionTree;
	/*changeToPostFix(symbolicString);
	generateTree(symbolicString, &expressionTree);*/

}

int main()
{
	printf("Hello, World!\n");
	return 0;
}

/* TODO: expression parsing:
 * convert string to symbolic string
 * convert infix symbolic string to postfix symbolic string
 * convert postfix symbolic string to tree
 * subtree breakdown
 */