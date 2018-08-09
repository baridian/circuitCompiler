//
// Created by ibird on 8/9/2018.
//

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "hashTable.h"

static const int START_SIZE = 4;

static unsigned hashString(hashTable table, void *key)
{
	char *keyCopy = (char *)key;
	unsigned toReturn = 0;
	int i = 0;
	for(; keyCopy[i];i++)
		toReturn += keyCopy[i] * pow(31,i);
	return toReturn % table.allocated;
}

static unsigned hash(hashTable table, void *key)
{
	unsigned toReturn = 0;
	int i = 0;
	for (; i < table.keySize; i++)
		toReturn += ((char *) key)[i] * pow(31, table.keySize - i - 1);
	return toReturn % table.allocated;
}

hashTable newHashTable(int keySize, int valueSize)
{
	hashTable toReturn;
	int i = 0;
	toReturn.allocated = START_SIZE;
	toReturn.indexesUsed = 0;
	toReturn.keySize = keySize;
	toReturn.valueSize = valueSize;
	toReturn.table = (hashNode *) malloc(sizeof(hashNode) * START_SIZE);
	for (; i < START_SIZE; i++)
		toReturn.table[i].isOccupied = 0;
	return toReturn;
}

static int memcmpr(void *a, void *b, int length)
{
	int i;
	if (a == NULL)
	{
		if (b == NULL)
			return 1;
		else
			return 0;
	}
	for (i = 0; i < length; i++)
	{
		if (*((char *) a) == *((char *) b))
		{
			(char *) a++;
			(char *) b++;
		}
		else
			return 0;
	}
	return 1;
}

void *readHash(hashTable table, void *key)
{
	int index;
	int start;
	if(table.keySize != STRING_SIZE)
		index = hash(table,key);
	else
		index = hashString(table,key);
	start = index;
	do
	{
		if(table.table[index].isOccupied)
		{
			if (table.keySize != STRING_SIZE && memcmpr(key, table.table[index].key, table.keySize))
				return table.table[index].data;
			else if (table.keySize == STRING_SIZE && strcmp((char *) table.table[index].key, (char *) key) == 0)
				return table.table[index].data;
		}
		index = index == table.allocated - 1 ? 0 : index + 1;
	} while (table.table[index].isOccupied && index != start);
	return NULL;
}

static void rescaleHash(hashTable *table)
{
	hashNode *oldTable;
	int oldLength;
	int i = 0;
	int newIndex;
	if (table->indexesUsed > table->allocated * 0.7 || table->indexesUsed < table->allocated * 0.7 / 2)
	{
		oldTable = table->table;
		oldLength = table->allocated;
		if(table->indexesUsed > table->allocated * 0.7)
			table->allocated *= 2;
		else if(table->allocated > START_SIZE)
			table->allocated /= 2;
		table->table = (hashNode *) malloc(sizeof(hashNode) * table->allocated);
		for (; i < table->allocated; i++)
			table->table[i].isOccupied = 0;
		for (i = 0; i < oldLength; i++)
		{
			if (oldTable[i].isOccupied)
			{
				if(table->keySize != STRING_SIZE)
					newIndex = hash(*table, oldTable[i].key);
				else
					newIndex = hashString(*table,oldTable[i].key);
				while (table->table[newIndex].isOccupied)
					newIndex = newIndex == table->allocated - 1 ? 0 : newIndex + 1;
				table->table[newIndex] = oldTable[i];
			}
		}
		free(oldTable);
	}
}

void writeHash(hashTable *table, void *key, void *value)
{
	int index;
	int start;
	if(table->keySize != STRING_SIZE)
		index = hash(*table,key);
	else
		index = hashString(*table,key);
	start = index;
	while (table->table[index].isOccupied)
	{
		if(table->keySize != STRING_SIZE && memcmpr(table->table[index].key,key,table->keySize))
			break;
		else if(table->keySize == STRING_SIZE && strcmp(table->table[index].key,key) == 0)
			break;
		index = index == table->allocated - 1 ? 0 : index + 1;
		if(index == start)
		{
			fprintf(stderr,"ERROR: table is full. This should never happen\n");
			exit(1);
		}
	}
	table->table[index].isOccupied = 1;
	if(table->keySize != STRING_SIZE)
	{
		table->table[index].key = malloc((unsigned) table->keySize);
		memcpy(table->table[index].key, key, (unsigned) table->keySize);
	}
	else
	{
		table->table[index].key = malloc(strlen(key) + 1);
		strcpy(table->table[index].key,key);
	}
	table->table[index].data = malloc((unsigned) table->valueSize);
	memcpy(table->table[index].data, value, (unsigned) table->valueSize);
	table->indexesUsed++;
	rescaleHash(table);
}

void eraseHashNode(hashTable *table, void *key)
{
	int index = hash(*table, key);
	int start = index;
	while (table->table[index].isOccupied)
	{
		if(memcmpr(table->table[index].key,key,table->keySize))
			break;
		index = index == table->allocated - 1 ? 0 : index + 1;
		if(index == start)
		{
			fprintf(stderr,"ERROR: table is full. This should never happen\n");
			exit(1);
		}
	}
	table->table[index].isOccupied = 0;
	free(table->table[index].key);
	free(table->table[index].data);
	table->indexesUsed--;
	rescaleHash(table);
}

int contains(hashTable table, void *key)
{
	if(readHash(table,key) != NULL)
		return 1;
	return 0;
}

void freeHashTable(hashTable table)
{
	int i = 0;
	for (; i < table.allocated; i++)
	{
		if (table.table[i].isOccupied)
		{
			free(table.table[i].key);
			free(table.table[i].data);
		}
	}
	free(table.table);
}