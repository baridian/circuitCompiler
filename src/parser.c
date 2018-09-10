#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "data structures/hashTable.h"

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
		if (!first)
		{
			toReturn = arrayToll(NULL,sizeof(symbol),0);
			break;
		}
		if (inputOffset != llLength(input) - 1)
			second = *(char *) llread(input, inputOffset + 1);
		else
			second = '\0';
		if (isOperator(first, second, opTable, tableLength))
		{
			start.type = operator;
			start.data.operator[0] = first;
			if (isDoubleCharOperator(first, second, opTable, tableLength))
			{
				start.data.operator[1] = second;
				inputOffset++;
			}
			else
				start.data.operator[1] = '\0';
			start.data.operator[2] = '\0';
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
			if (second != '\0' || isOperator(*(char *)llread(variableName,-1),second,opTable,tableLength))
			{
				second = first;
				llErase(&variableName, -1);
			}
			first = '\0';
			llAppend(&variableName, &first);
			start.type = variable;
			start.data.variable = malloc(sizeof(char) * llLength(variableName));
			llToArray(variableName, start.data.variable);
			if (sscanf(start.data.variable, "%c%d", &tempChar, &tempInt) == 2 && tempChar == AUTO_VAR)
			{
				fprintf(stderr, "ERROR: used restricted variable name '%c%d'\n", tempChar, tempInt);
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
				if (opTable[i][j] == a.data.operator[0])
				{
					if (a.data.operator[1])
					{
						if (opTable[i][j + 1] == a.data.operator[1])
							aLevel = i;
					}
					else
						aLevel = i;
				}
				if (opTable[i][j] == b.data.operator[0])
				{
					if (b.data.operator[1])
					{
						if (opTable[i][j + 1] == b.data.operator[1])
							bLevel = i;
					}
					else
						bLevel = i;
				}
				if (opTable[i][j + 1] != ',' && opTable[i][j + 1] != '\0')
					j++;
			}
		}
	}
	return bLevel - aLevel;
}

static void autoWrap(symbol *array[], int *length)
{
	linkedList symbolList = arrayToll(*array, sizeof(symbol), *length);
	int i;
	symbol toInsert;
	toInsert.type = operator;
	toInsert.data.operator[1] = '\0';
	for (i = 0; i < llLength(symbolList); i++)
	{
		if ((*(symbol *) llread(symbolList, i)).data.operator[0] == '=')
		{
			toInsert.data.operator[0] = '(';
			llInsert(&symbolList, i - 1, &toInsert);
			toInsert.data.operator[0] = ')';
			llAppend(&symbolList, &toInsert);
			*length += 2;
			i++;
		}
	}
	*array = (symbol *) realloc(*array, sizeof(symbol) * *length);
	llToArray(symbolList, *array);
	freell(symbolList);
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
	symbol *temp = malloc(sizeof(symbol) * *length);
	symbol poppedSymbol;
	memcpy(temp, array, sizeof(symbol) * *length);
	autoWrap(&temp, length);
	for (tempCounter = 0; tempCounter < *length; tempCounter++)
	{
		if (temp[tempCounter].type == operator)
		{
			if (stackSize(symbolStack) > 0)
			{
				poppedSymbol = *(symbol *) speek(symbolStack);
				skip = 0;
			}
			if (temp[tempCounter].data.operator[0] == ')' && !skip)
			{
				/*pop until opening parenthesis found*/
				while (stackSize(symbolStack) > 0 && poppedSymbol.data.operator[0] != '(')
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
					   poppedSymbol.data.operator[0] != '(' && temp[tempCounter].data.operator[0] != '(')
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
		if (poppedSymbol.data.operator[0] == '(')
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
 * each time a literal or operator is found, push it to the stack as a tree consisting of just the operator root.
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
				fprintf(stderr, "ERROR: missing rValue\n");
				exit(1);
			}
		}
		spush(&tempRoot, &treeStack);
	}
	if (stackSize(treeStack) != 1)
	{
		fprintf(stderr, "ERROR: missing lValue\n");
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
static int atomizeTree(tree expressionTree, expression **output)
{
	char *remember;
	int expressionCount = 0;
	symbol operator;
	symbol lValue;
	symbol rValue;
	symbol newSymbol;
	linkedList expressions;
	expression newExpression;
	treeDir directionToTrim;
	resetToRoot(&expressionTree);
	while (currentNodeType(expressionTree) == dualInternal) /*while tree is not totally broken down*/
	{
		stepToLowestInternal(&expressionTree); /*populate atomic expression data from lowest parent / child set*/
		step(&expressionTree, up);
		operator = *(symbol *) readNode(expressionTree);
		step(&expressionTree, left);
		lValue = *(symbol *) readNode(expressionTree);
		step(&expressionTree, up);
		step(&expressionTree, right);
		rValue = *(symbol *) readNode(expressionTree);

		/*for the atomic expression data, format it and print it to the output*/
		if (operator.data.operator[0] != '=')
		{

			/*create node for the variable assigned to the atomic expression*/
			newSymbol.type = variable;
			newSymbol.data.variable = malloc((unsigned) (2 + expressionCount / 10));
			sprintf(newSymbol.data.variable, "%c%d\0", AUTO_VAR, expressionCount);
			newExpression.isTrivial = 0;
			newExpression.assignTo = newSymbol;
			newExpression.lValue = lValue;
			newExpression.operator = operator;
			newExpression.rValue = rValue;
		}
		else
		{
			if (lValue.type != variable)
			{
				fprintf(stderr, "attempted to assign value to literal\n");
				exit(1);
			}
			newExpression.isTrivial = 1;
			newSymbol.type = variable;
			newSymbol.data.variable = malloc(strlen(lValue.data.variable) + 1);
			strcpy(newSymbol.data.variable, lValue.data.variable);
			newExpression.assignTo = newSymbol;
			newExpression.rValue = rValue;
		}

		if (expressionCount == 0)
		{
			expressions = arrayToll(&newExpression, sizeof(expression), 1);
			expressionCount++;
		}
		else
		{
			llAppend(&expressions, &newExpression);
			if (!newExpression.isTrivial)
				expressionCount++;
		}

		if (operator.data.operator[0] == '=')
		{
			if (rValue.type == variable)
			{
				newSymbol.data.variable = malloc((strlen(rValue.data.variable) + 1) * sizeof(char));
				strcpy(newSymbol.data.variable, rValue.data.variable);
			}
			else
				newSymbol = rValue;
		}
		else
		{
			remember = newSymbol.data.variable;
			newSymbol.data.variable = malloc((strlen(remember) + 1) * sizeof(char));
			strcpy(newSymbol.data.variable, remember);
		}

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
		/*reset to root and start again*/
		resetToRoot(&expressionTree);
	}
	expressionCount = llLength(expressions);
	*output = malloc(sizeof(expression) * expressionCount);
	llToArray(expressions, *output);
	freell(expressions);
	return expressionCount;
}

static void simplifyExpressionArray(expression expressions[], int *length)
{
	hashTable trivialExpressions = newHashTable(STRING_SIZE, sizeof(char *));
	hashTable toRemove = newHashTable(sizeof(int), sizeof(char *));
	linkedList expressionList = arrayToll(expressions, sizeof(expression), *length);
	expression current;
	char *copy;
	int i = 0;
	for (; i < llLength(expressionList); i++)
	{
		current = *(expression *) llread(expressionList, i);
		if (current.isTrivial && current.rValue.type == variable)
		{
			if (!contains(trivialExpressions, current.rValue.data.variable))
			{
				copy = malloc((strlen(current.assignTo.data.variable) + 1) * sizeof(char));
				strcpy(copy, current.assignTo.data.variable);
				writeHash(&trivialExpressions, current.rValue.data.variable, &copy);
			}
			else
			{
				copy = malloc((strlen(current.rValue.data.variable) + 1) * sizeof(char));
				strcpy(copy, current.rValue.data.variable);
				writeHash(&toRemove, &i, &copy);
			}
		}
	}
	for (i = 0; i < llLength(expressionList); i++)
	{
		if (contains(toRemove, &i))
		{
			copy = *(char **) readHash(toRemove, &i);
			eraseHashNode(&trivialExpressions, copy);
		}
	}
	freeHashTable(toRemove);
	for (i = 0; i < llLength(expressionList); i++)
	{
		current = *(expression *) llread(expressionList, i);
		if (current.rValue.type == variable && current.isTrivial)
		{
			if (contains(trivialExpressions, current.rValue.data.variable))
			{
				copy = *(char **) readHash(trivialExpressions, current.rValue.data.variable);
				if (strcmp(copy, current.assignTo.data.variable) == 0)
				{
					llErase(&expressionList, i);
					i--;
					continue;
				}
			}
		}
		if (contains(trivialExpressions, current.assignTo.data.variable))
		{
			copy = *(char **) readHash(trivialExpressions, current.assignTo.data.variable);
			current.assignTo.data.variable = malloc((strlen(copy) + 1) * sizeof(char));
			strcpy(current.assignTo.data.variable, copy);
			llErase(&expressionList, i);
			llInsert(&expressionList, i, &current);
		}
		if (current.rValue.type == variable && contains(trivialExpressions, current.rValue.data.variable))
		{
			copy = *(char **) readHash(trivialExpressions, current.rValue.data.variable);
			current.rValue.data.variable = malloc((strlen(copy) + 1) * sizeof(char));
			strcpy(current.rValue.data.variable, copy);
			llErase(&expressionList, i);
			llInsert(&expressionList, i, &current);
		}
		if (current.lValue.type == variable && contains(trivialExpressions, current.lValue.data.variable))
		{
			copy = *(char **) readHash(trivialExpressions, current.lValue.data.variable);
			current.lValue.data.variable = malloc((strlen(copy) + 1) * sizeof(char));
			strcpy(current.lValue.data.variable, copy);
			llErase(&expressionList, i);
			llInsert(&expressionList, i, &current);
		}
	}
	*length = llLength(expressionList);
	llToArray(expressionList, expressions);
	freell(expressionList);
	freeHashTable(trivialExpressions);
}

static void expressionArrayToString(expression expressions[], int length, char *output)
{
	int i;
	int outputOffset = 0;
	symbol lValue;
	symbol rValue;
	symbol operator;
	symbol assign;
	for (i = 0; i < length; i++)
	{
		lValue = expressions[i].lValue;
		rValue = expressions[i].rValue;
		operator = expressions[i].operator;
		assign = expressions[i].assignTo;
		if (!expressions[i].isTrivial)
		{
			sprintf(output + outputOffset, "%s=%s%d%s%s%s%d%s;", assign.data.variable,
					lValue.type == variable ? lValue.data.variable : "",
					lValue.type == literal ? lValue.data.literal : 1,
					lValue.type == literal ? "" : "\b",
					operator.data.operator, rValue.type == variable ? rValue.data.variable : "",
					rValue.type == literal ? rValue.data.literal : 1,
					rValue.type == literal ? "" : "\b");
		}
		else
		{
			sprintf(output + outputOffset, "%s=%d%s%s;", assign.data.variable,
					rValue.type == literal ? rValue.data.literal : 1,
					rValue.type == literal ? "" : "\b",
					rValue.type == variable ? rValue.data.variable : "");
		}
		while (output[++outputOffset]);
	}
	parseBackspaces(output);
}

static void freeExpressionArray(expression *expressions, int length)
{
	int i = 0;
	for (; i < length; i++)
	{
		free(expressions[i].assignTo.data.variable);
		if (expressions[i].lValue.type == variable && !expressions[i].isTrivial)
			free(expressions[i].lValue.data.variable);
		if (expressions[i].rValue.type == variable)
			free(expressions[i].rValue.data.variable);
	}
	free(expressions);
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
	symbol *symbols = malloc(llLength(symbolicList) * sizeof(symbol));
	tree expressionTree;
	expression *expressions;
	freell(charList);
	llToArray(symbolicList, symbols);
	freell(symbolicList);
	infixToPostfix(symbols, &length, opTable, tableLength);
	postfixToTree(symbols, length, &expressionTree);
	length = atomizeTree(expressionTree, &expressions);
	simplifyExpressionArray(expressions, &length);
	expressionArrayToString(expressions, length, output);
	freeExpressionArray(expressions, length);
	return length;
}