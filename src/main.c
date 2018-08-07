#include <stdio.h>
#include "parser.h"


int main()
{
	char output[300] = {'\0'};
	char *opTable[] = {"**", "*,/", "+,-", "(,)"};
	int tableLength = 4;
	convertExpression("d1/d2+(d3-d4/5)+18*3", output, opTable, tableLength);
	printf("%s", output);
	return 0;
}