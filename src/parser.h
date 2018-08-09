//
// Created by ibird on 8/7/2018.
//

#ifndef CRCT_PARSER_H
#define CRCT_PARSER_H

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

typedef struct expression
{
	unsigned isTrivial : 1;
	symbol assignTo;
	symbol leftOperand;
	symbol operator;
	symbol rightOperand;
} expression;

static const char AUTO_VAR = 'k';

static int isOperator(char first, char second, char *opTable[], int tableLength);

static int isDoubleCharOperator(char a, char b, char *opTable[], int tableLength);

static int isLiteral(char c);

static linkedList charListToSymbolList(linkedList input, char *opTable[], int tableLength);

static int comparePrecedence(symbol a, symbol b, char *opTable[], int tableLength);

static void autoWrap(symbol *array[], int *length);

static void infixToPostfix(symbol array[], int *length, char *opTable[], int tableLength);

static void postfixToTree(symbol symbols[], int length, tree *expressionTree);

static int atomizeTree(tree expressionTree, expression **output);

static void simplifyExpressionArray(expression expressions[], int *length);

static void expressionArrayToString(expression expressions[], int length, char *output);

static void freeExpressionArray(expression *expressions, int length);

/*
 * converts a complex expression character string into a series of atomic expressions in a character string.
 * The variable containing the value equal to the result of the original complex expression is c#,
 * with # being the value returned by the function
 */
int convertExpression(char *input, char *output, char *opTable[], int tableLength);

#endif //CRCT_PARSER_H
