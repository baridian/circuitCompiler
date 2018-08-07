//
// Created by ibird on 8/7/2018.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

static int isOperator(char first, char second, char *opTable[], int tableLength)
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

static int isDoubleCharOperator(char a, char b, char *opTable[], int tableLength)
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

static int isLiteral(char c)
{
	return '0' <= c && c <= '9';
}

static linkedList charListToSymbolList(linkedList input, char *opTable[], int tableLength)
{
	int inputOffset;
	int initialized = 0;
	int literalVal;
	int tempInt;
	char first, second;
	char tempChar;
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
			if(sscanf(start.data.variable,"%c%d",&tempChar,&tempInt) == 2 && tempChar == AUTO_VAR)
			{
				fprintf(stderr,"ERROR: used restricted variable name '%c%d'\n",tempChar,tempInt);
				exit(1);
			}
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
static int comparePrecedence(symbol a, symbol b, char *opTable[], int tableLength)
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
				if(opTable[i][j+1] != ',' && opTable[i][j+1] != '\0')
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
static void infixToPostfix(symbol array[], int *length, char *opTable[], int tableLength)
{
	stack symbolStack = newStack(sizeof(symbol));
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
					   comparePrecedence(temp[tempCounter], poppedSymbol, opTable, tableLength) <= 0 &&
					   poppedSymbol.data.operand[0] != '(' && temp[tempCounter].data.operand[0] != '(')
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
static void postfixToTree(symbol symbols[], int length, tree *expressionTree)
{
	stack treeStack = newStack(sizeof(tree));
	tree tempRoot;
	tree tempLeft;
	tree tempRight;
	int symbolOffset;
	for (symbolOffset = 0; symbolOffset < length; symbolOffset++)
	{
		tempRoot = newTree(sizeof(symbol), symbols + symbolOffset);
		if (symbols[symbolOffset].type == operator)
		{
			if (stackSize(treeStack) >= 2)
			{
				tempRight = *(tree *) spop(&treeStack);
				tempLeft = *(tree *) spop(&treeStack);
				splice(tempRoot, right, tempRight);
				splice(tempRoot, left, tempLeft);
			}
			else
			{
				fprintf(stderr, "ERROR: Invalid expression\n");
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

static void parseBackspaces(char *string)
{
	int length = (int) strlen(string);
	int index;
	linkedList charList = arrayToll(string, sizeof(char), length);
	linkedList matchData = arrayToll("\b", sizeof(char), 1);
	while ((index = llMatch(charList, matchData)) != -1)
	{ /*when a backspace is found replace it and the character before it*/
		llErase(&charList, index - 1);
		llErase(&charList, index - 1);
	}
	llAppend(&charList, "\0");
	llToArray(charList, string);
	freell(charList);
	freell(matchData);
}

/* breaks down an expression tree into a character string of atomic statements, that when solved will return a value
 * equal to the original expression within the variable c#, with # being the value returned by the function
 */
static int atomizeTree(tree expressionTree, char *output)
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
		sprintf(output + outputOffset, "%c%d=%s%d%s%s%s%d%s;", AUTO_VAR,expressionCount++,
				leftOperand.type == variable ? leftOperand.data.variable : "",
				leftOperand.type == literal ? leftOperand.data.literal : 1, leftOperand.type == literal ? "" : "\b",
				operator.data.operand, rightOperand.type == variable ? rightOperand.data.variable : "",
				rightOperand.type == literal ? rightOperand.data.literal : 1, rightOperand.type == literal ? "" : "\b");

		/*create node for the variable assigned to the atomic expression*/
		newSymbol.type = variable;
		newSymbol.data.variable = (char *) malloc((unsigned) (2 + expressionCount / 10));
		sprintf(newSymbol.data.variable, "%c%d",AUTO_VAR, expressionCount - 1);

		/*free the old atomic expression*/
		if (leftOperand.type == variable)
			free(leftOperand.data.variable);
		if (rightOperand.type == variable)
			free(rightOperand.data.variable);
		step(&expressionTree, up);
		directionToTrim = stepUpAndGetStepToPrevious(&expressionTree);
		/*if this is not the final assignment put variable in place of old atomic expression, else free*/
		if (step(&expressionTree, directionToTrim), currentNodeType(expressionTree) != leaf)
		{
			step(&expressionTree, up);
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
	return outputOffset;
}

void printSymbolicString(symbol array[], int length)
{
	int i = 0;
	for (; i < length; i++)
	{
		switch (array[i].type)
		{
			case variable:
				printf("V:%s ", array[i].data.variable);
				break;
			case literal:
				printf("L:%d ", array[i].data.literal);
				break;
			case operator:
				printf("O:%s ", array[i].data.operand);
				break;
			default:
				fprintf(stderr, "ERROR: Unrecognized type\n");
		}
	}
	printf("\n");
}


/*
 * converts a complex expression character string into a series of atomic expressions in a character string.
 * The variable containing the value equal to the result of the original complex expression is c#,
 * with # being the value returned by the function
 */
int convertExpression(char *input, char *output, char *opTable[], int tableLength)
{
	linkedList charList = arrayToll(input, sizeof(char), strlen(input) + 1);
	linkedList symbolicList = charListToSymbolList(charList, opTable, tableLength);
	int length = llLength(symbolicList);
	symbol *symbols = (symbol *) malloc(llLength(symbolicList) * sizeof(symbol));
	tree expressionTree;
	freell(charList);
	llToArray(symbolicList, symbols);
	freell(symbolicList);
	printSymbolicString(symbols,length);
	infixToPostfix(symbols, &length, opTable, tableLength);
	printSymbolicString(symbols, length);
	postfixToTree(symbols, length, &expressionTree);
	free(symbols);
	return atomizeTree(expressionTree, output);
}