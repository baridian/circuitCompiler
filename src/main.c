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

int isOperator(char first, char second, char *opTable[], int tableLength)
{
	int i, j;
	for (i = 0; i < tableLength; i++)
	{
		for (j = 0; opTable[i][j]; j++)
		{
			if (opTable[i][j] == ',')
				continue;
			if (first == opTable[i][j])
			{
				if (opTable[i][j + 1] != ',' && opTable[i][j + 1] != '\0')
				{
					if (second == opTable[i][j + 1])
						return 1;
					j++;
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
			{
				start.data.operand[1] = second;
				inputOffset++;
			}
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
			for (literalVal = 0; first = *(char *) llread(input, inputOffset), isLiteral(first); inputOffset++)
			{
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
			if (first != '\0')
				inputOffset--;
			else
				break;
		}
		else
		{
			variableName = arrayToll(&first, sizeof(char), 1);
			do
			{
				first = *(char *) llread(input, inputOffset);
				if (inputOffset != llLength(input) - 1)
					second = *(char *) llread(input, inputOffset + 1);
				llAppend(&variableName, &first);
				inputOffset++;
			} while (!isOperator(first, second, opTable, tableLength) && inputOffset + 1 < llLength(input));
			llErase(&variableName, 0);
			if (second != '\0')
				llErase(&variableName, -1);
			first = '\0';
			llAppend(&variableName, &first);
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
			if (second != '\0')
				inputOffset -= 2;
			else
				break;
		}
	}
	return toReturn;
}

/*returns positive if a has a higher precedence, 0 if equal, negative if b has higher*/
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
				j++;
			}
		}
	}
	return bLevel - aLevel;
}

/*
 * if symbol or literal encountered, goes straight to output buffer. If operator encountered,
 * operators are popped off and appended to the output until a operator of lower precedence is found,
 * then the current operator is pushed on. When a right parenthesis is encountered, all the symbols are popped off
 * and appended to the output except the left parenthesis, which is popped off but not appended.
 * When the input buffer is exhausted, all remaining operators on the stack are popped off and appended. If a parenthesis
 * is encountered during this stage, an error is thrown.
 */
void changeToPostFix(symbol array[], int *length, char *opTable[], int tableLength)
{
	stack symbolStack = newStack(sizeof(symbol));;
	int tempCounter;
	int skip = 1;
	int outputCounter = 0;
	int parenthesisCount = 0;
	symbol *temp = (symbol *) malloc(sizeof(symbol) * *length);
	symbol poppedSymbol;
	memcpy(temp, array, sizeof(symbol) * *length);
	for (tempCounter = 0; tempCounter < *length; tempCounter++)
	{
		if (temp[tempCounter].type == operator)
		{
			if (stackSize(symbolStack) > 0)
			{
				poppedSymbol = *(symbol *) speek(symbolStack);
				skip = 0;
			}
			if (temp[tempCounter].data.operand[0] == ')' && !skip)
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
				spop(&symbolStack);
				parenthesisCount += 2;
			}
			else
			{
				/*pop until lower precedence found*/
				while (stackSize(symbolStack) > 0 &&
					   comparePrecedence(temp[tempCounter], poppedSymbol, opTable, tableLength) >= 0 &&
					   poppedSymbol.data.operand[0] != '(')
				{
					array[outputCounter++] = poppedSymbol;
					spop(&symbolStack);
					if (stackSize(symbolStack) > 0)
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
	while (stackSize(symbolStack) > 0) /*clear out stack after symbols end*/
	{
		poppedSymbol = *(symbol *) spop(&symbolStack);
		if (poppedSymbol.data.operand[0] == '(')
		{
			fprintf(stderr, "ERROR: unclosed parenthesis\n");
			exit(1);
		}
		array[outputCounter++] = poppedSymbol;
	}
	free(temp);
	frees(symbolStack);
	*length -= parenthesisCount;
}

/*
 * each time a literal or operand is found, push it to the stack as a tree consisting of just the operand root.
 * When an operator is found, it pops the two most recent trees off the stack and splices them to a tree with
 * the operator as the root and the two most recent trees as it's children.
 * Repeat until string parsed, if stack != 1, error.
 */
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
			if(stackSize(treeStack) >= 2)
			{
				tempRight = *(tree *) spop(&treeStack);
				tempLeft = *(tree *) spop(&treeStack);
				splice(tempRoot, right, tempRight);
				splice(tempRoot, left, tempLeft);
			}
			else
			{
				fprintf(stderr,"ERROR: Invalid expression\n");
				exit(1);
			}
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
	int length = (int) strlen(string);
	int index;
	linkedList charString = arrayToll(string, sizeof(char), length);
	linkedList matchData = arrayToll("\b", sizeof(char), 1);
	while ((index = llMatch(charString, matchData)) != -1)
	{ /*when a backspace is found replace it and the character before it*/
		llErase(&charString, index - 1);
		llErase(&charString, index - 1);
	}
	llAppend(&charString, "\0");
	llToArray(charString, string);
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
	while (currentNodeType(expressionTree) == dualInternal) /*while tree is not totally broken down*/
	{
		stepToLowestInternal(&expressionTree); /*populate atomic expression data from lowest parent / child set*/
		step(&expressionTree, up);
		operator = *(symbol *) readNode(expressionTree);
		step(&expressionTree, left);
		leftOperand = *(symbol *) readNode(expressionTree);
		step(&expressionTree, up);
		step(&expressionTree, right);
		rightOperand = *(symbol *) readNode(expressionTree);

		/*for the atomic expression data, format it and print it to the output*/
		sprintf(output + outputOffset, "c%d=%s%d%s%s%s%d%s;", expressionCount++,
				leftOperand.type == variable ? leftOperand.data.variable : "",
				leftOperand.type == literal ? leftOperand.data.literal : 1, leftOperand.type == literal ? "" : "\b",
				operator.data.operand, rightOperand.type == variable ? rightOperand.data.variable : "",
				rightOperand.type == literal ? rightOperand.data.literal : 1, rightOperand.type == literal ? "" : "\b");

		/*create node for the variable assigned to the atomic expression*/
		newSymbol.type = variable;
		newSymbol.data.variable = (char *) malloc((unsigned) (2 + expressionCount / 10));
		sprintf(newSymbol.data.variable, "c%d", expressionCount - 1);

		/*free the old atomic expression*/
		if (leftOperand.type == variable)
			free(leftOperand.data.variable);
		if (rightOperand.type == variable)
			free(rightOperand.data.variable);
		step(&expressionTree, up);
		directionToTrim = stepUpAndGetStepToPrevious(&expressionTree);
		/*if this is not the final assignment put variable in place of old atomic expression, else free*/
		if (step(&expressionTree,directionToTrim),currentNodeType(expressionTree) != leaf)
		{
			step(&expressionTree,up);
			trim(&expressionTree, directionToTrim);
			addNode(expressionTree, directionToTrim, &newSymbol);
		}
		else
		{
			freeTree(expressionTree);
			break;
		}
		while (output[++outputOffset]);
		/*reset to root and start again*/
		resetToRoot(&expressionTree);
	}
	parseBackspaces(output);
}

void convertExpression(char *input, char *output, char *opTable[], int tableLength)
{
	linkedList charString = arrayToll(input, sizeof(char), strlen(input) + 1);
	linkedList symbolicList = generateSymbolicList(charString, opTable, tableLength);
	int length = llLength(symbolicList);
	symbol *symbolicString = (symbol *) malloc(llLength(symbolicList) * sizeof(symbol));
	tree expressionTree;
	llToArray(symbolicList, symbolicString);
	freell(charString);
	freell(symbolicList);
	changeToPostFix(symbolicString, &length, opTable, tableLength);
	generateTree(symbolicString, length, &expressionTree);
	free(symbolicString);
	breakDownTree(expressionTree, output);
}

int main()
{
	char output[300] = { '\0' };
	char *opTable[] = {"**", "*,/", "+,-", "(,)"};
	int tableLength = 4;
	convertExpression("d1/d2+(d3-d4/5)+18*3", output, opTable, tableLength);
	printf("%s",output);
	return 0;
}