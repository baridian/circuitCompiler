#include <stdio.h>
#include "parser.h"

char *OP_TABLE[] = {"!","**","*,/,%","+,-",">>,<<",">,>=,<,<=","==","&","^","|","&&","||","=","(,)"};
const int TABLE_LENGTH = 14;

int main()
{
	char output[300] = {'\0'};
	convertExpression("test=(uOre>2000||acid<1000)&&acid>1", output, OP_TABLE, TABLE_LENGTH);
	printf("%s", output);
	return 0;
}