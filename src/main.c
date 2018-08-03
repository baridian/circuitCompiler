#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "data structures/linkedList.h"
#include "data structures/tree.h"

typedef enum symbolType
{
	literal, variable, operator
} symbolType;

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

int isLiteral(char c)
{
	return '0' <= c && c <= '9';
}

linkedList generateSymbolicList(linkedList input, char *opTable[], int tableLength)
{
	int inputOffset;
	int initialized = 0;
	int literalVal;
	char first, second;
	symbol start;
	linkedList variableName;
	linkedList toReturn;
	for (inputOffset = 0; inputOffset < llLength(input); inputOffset++)
	{
		first = *(char *) llread(input, inputOffset);
		if (inputOffset != llLength(input) - 1)
			second = *(char *) llread(input, inputOffset + 1);
		else
			second = '\0';
		if (isOperator(first, second, opTable, tableLength))
		{
			start.type = operator;
			start.data.operand[0] = first;
			if (isDoubleCharOperator(first, second, opTable, tableLength))
				start.data.operand[1] = second;
			else
				start.data.operand[1] = '\0';
			start.data.operand[2] = '\0';
			if (initialized)
				llAppend(&toReturn, &start);
			else
			{
				toReturn = arrayToll(&start, sizeof(symbol), 1);
				initialized = 1;
			}
		}
		else if (isLiteral(first))
		{
			literalVal = 0;
			for (; isLiteral(first); inputOffset++)
			{
				first = *(char *) llread(input, inputOffset);
				literalVal *= 10;
				literalVal += first - '0';
			}
			start.type = literal;
			start.data.literal = literalVal;
			if (initialized)
				llAppend(&toReturn, &start);
			else
			{
				toReturn = arrayToll(&start, sizeof(symbol), 1);
				initialized = 1;
			}
		}
		else
		{
			variableName = arrayToll(&first, sizeof(char), 1);
			for (; !isLiteral(first) && !isOperator(first, second, opTable, tableLength) &&
				   inputOffset < llLength(input); inputOffset++)
			{
				llAppend(&variableName, &first);
				first = *(char *) llread(input, inputOffset);
				if (inputOffset != llLength(input) - 1)
					second = *(char *) llread(input, inputOffset + 1);
			}
			llErase(&variableName, 0);
			start.type = variable;
			start.data.variable = (char *) malloc(sizeof(char) * llLength(variableName));
			llToArray(variableName, start.data.variable);
			freell(variableName);
			if (initialized)
				llAppend(&toReturn, &start);
			else
			{
				toReturn = arrayToll(&start, sizeof(symbol), 1);
				initialized = 1;
			}
		}
	}
	return toReturn;
}

int comparePrecedence(symbol a, symbol b, char *opTable[], int tableLength)
{
	int i, j, aLevel, bLevel;
	for (i = 0; i < tableLength; i++)
	{
		for (j = 0; opTable[i][j]; j++)
		{
			if (opTable[i][j] != ',')
			{
				if (opTable[i][j] == a.data.operand[0])
				{
					if (a.data.operand[1])
					{
						if (opTable[i][j + 1] == a.data.operand[1])
							aLevel = i;
					}
					else
						aLevel = i;
				}
				if (opTable[i][j] == b.data.operand[0])
				{
					if (b.data.operand[1])
					{
						if (opTable[i][j + 1] == b.data.operand[1])
							bLevel = i;
					}
					else
						bLevel = i;
				}
			}
		}
	}
	return bLevel - aLevel;
}

void changeToPostFix(symbol array[], int length, char *opTable[], int tableLength)
{
	stack symbolStack = newStack(sizeof(symbol));
	int tempCounter = 0;
	int outputCounter = 0;
	symbol *temp = (symbol *) malloc(sizeof(symbol) * length);
	symbol poppedSymbol;
	memcpy(temp, array, sizeof(symbol) * length);
	for (tempCounter = 0; tempCounter < length; tempCounter++)
	{
		if (temp[tempCounter].type == operator)
		{
			poppedSymbol = *(symbol *) speek(symbolStack);
			if (poppedSymbol.data.operand[0] == ')')
			{
				/*pop until opening parenthesis found*/
				while (stackSize(symbolStack) > 0 && poppedSymbol.data.operand[0] != '(')
				{
					array[outputCounter++] = poppedSymbol;
					spop(&symbolStack);
					poppedSymbol = *(symbol *) speek(symbolStack);
				}
				if (stackSize(symbolStack) == 0)
				{
					fprintf(stderr, "ERROR: opening parenthesis not found");
					exit(1);
				}
				else
					spop(&symbolStack);
			}
			else
			{
				/*pop until lower precedence found*/
				while (stackSize(symbolStack) > 0 &&
					   comparePrecedence(temp[tempCounter], poppedSymbol, opTable, tableLength) < 1)
				{
					array[outputCounter++] = poppedSymbol;
					spop(&symbolStack);
					poppedSymbol = *(symbol *) speek(symbolStack);
				}
				spush(temp + tempCounter, &symbolStack);
			}
		}
		else
		{
			array[outputCounter++] = temp[tempCounter];
		}
	}
	free(temp);
	frees(symbolStack);
}

void generateTree(symbol symbolicString[], int length, tree *expressionTree)
{
	stack treeStack = newStack(sizeof(tree));
	tree tempRoot;
	tree tempLeft;
	tree tempRight;
	int symbolOffset;
	for (symbolOffset = 0; symbolOffset < length; symbolOffset++)
	{
		tempRoot = newTree(sizeof(symbol), symbolicString + symbolOffset);
		if (symbolicString[symbolOffset].type == operator)
		{
			tempRight = *(tree *) spop(&treeStack);
			tempLeft = *(tree *) spop(&treeStack);
			splice(tempRoot, right, tempRight);
			splice(tempRoot, left, tempLeft);
		}
		spush(&tempRoot, &treeStack);
	}
	if (stackSize(treeStack) != 1)
	{
		fprintf(stderr, "ERROR: Invalid expression\n");
		exit(1);
	}
	*expressionTree = *(tree *) spop(&treeStack);
	frees(treeStack);
}

void parseBackspaces(char *string)
{
	int length = (int)strlen(string);
	int index;
	linkedList charString = arrayToll(string,sizeof(char),length);
	linkedList matchData = arrayToll("\b",sizeof(char),1);
	while((index = llMatch(charString,matchData)) != -1)
	{
		llErase(&charString,index);
		llErase(&charString,index);
	}
	llAppend(&charString,"\0");
	llToArray(charString,string);
	freell(charString);
	freell(matchData);
}

void breakDownTree(tree expressionTree, char *output)
{
	int outputOffset = 0;
	int expressionCount = 0;
	symbol operator;
	symbol leftOperand;
	symbol rightOperand;
	symbol newSymbol;
	treeDir directionToTrim;
	resetToRoot(&expressionTree);
	while (currentNodeType(expressionTree) == dualInternal)
	{
		stepToLowestInternal(&expressionTree);
		step(&expressionTree, up);
		operator = *(symbol *) readNode(expressionTree);
		step(&expressionTree, left);
		leftOperand = *(symbol *) readNode(expressionTree);
		step(&expressionTree, up);
		step(&expressionTree, right);
		rightOperand = *(symbol *) readNode(expressionTree);
		sprintf(output + outputOffset, "%c=%s%d%s%s%s%d%s;", expressionCount++,
				leftOperand.type == variable ? leftOperand.data.variable : "",
				leftOperand.type == literal ? leftOperand.data.literal : 1, leftOperand.type == literal ? "" : "\b",
				operator.data.operand, rightOperand.type == variable ? rightOperand.data.variable : "",
				rightOperand.type == literal ? rightOperand.data.literal : 1, rightOperand.type == literal ? "" : "\b");
		newSymbol.type = variable;
		newSymbol.data.variable = (char *)malloc((unsigned)(2 + expressionCount / 10));
		sprintf(newSymbol.data.variable,"c%d",expressionCount);
		if(leftOperand.type == variable)
			free(leftOperand.data.variable);
		if(rightOperand.type == variable)
			free(rightOperand.data.variable);
		step(&expressionTree, up);
		directionToTrim = stepUpAndGetStepToPrevious(&expressionTree);
		if(!isRoot(expressionTree))
		{
			trim(&expressionTree, directionToTrim);
			addNode(expressionTree, directionToTrim, &newSymbol);
		}
		else
			freeTree(expressionTree);
		while(output[outputOffset++]);
		resetToRoot(&expressionTree);
	}
	parseBackspaces(output);
}

void convertExpression(char *input, char *output, char *opTable[], int tableLength)
{
	linkedList charString = arrayToll(input, sizeof(char), sizeof(input));
	linkedList symbolicList = generateSymbolicList(charString, opTable, tableLength);
	int length = llLength(symbolicList);
	symbol *symbolicString = (symbol *) malloc(llLength(symbolicList) * sizeof(symbol));
	tree expressionTree;
	llToArray(symbolicList, symbolicString);
	freell(charString);
	freell(symbolicList);
	changeToPostFix(symbolicString, length, opTable, tableLength);
	free(symbolicString);
	generateTree(symbolicString, length, &expressionTree);
	breakDownTree(expressionTree,output);
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