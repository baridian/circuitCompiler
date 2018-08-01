#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tree.h"
#include "stack.h"

/*TODO: add in fix for null root */

tree newTree(int dataSize)
{
	tree toReturn;
	toReturn.root = NULL;
	toReturn.dataSize = (unsigned) dataSize;
	toReturn.current = NULL;
	return toReturn;
}

void addNode(tree t, treeDir dir, void *data)
{
	switch (dir)
	{
		case left:
			if (t.current->left != NULL)
			{
				fprintf(stderr, "ERROR: attempted to add child node to non-leaf\n");
				exit(1);
			}
			else
			{
				t.current->left = (treeNode *) malloc(sizeof(treeNode));
				t.current->left->parent = t.current;
				t.current->left->left = NULL;
				t.current->left->right = NULL;
				memcpy(t.current->left->data, data, t.dataSize);
			}
		case right:
			if (t.current->right != NULL)
			{
				fprintf(stderr, "ERROR: attempted to add child node to non-leaf\n");
				exit(1);
			}
			else
			{
				t.current->right = (treeNode *) malloc(sizeof(treeNode));
				t.current->right->parent = t.current;
				t.current->right->left = NULL;
				t.current->right->right = NULL;
				memcpy(t.current->right->data, data, t.dataSize);
			}
		case up:
			if (t.current->parent != NULL)
			{
				fprintf(stderr, "ERROR: attempted to add parent node to non-root\n");
				exit(1);
			}
			else
			{
				t.current->parent = (treeNode *) malloc(sizeof(treeNode));
				t.current->parent->left = t.current;
				t.current->parent->parent = NULL;
				t.current->parent->right = NULL;
				memcpy(t.current->parent->data, data, t.dataSize);
			}
	}
}

void step(tree *t, treeDir dir)
{
	switch(dir)
	{
		case up:
			if(t->current->parent != NULL)
				t->current = t->current->parent;
		case left:
			if(t->current->left != NULL)
				t->current = t->current->left;
		case right:
			if(t->current->right != NULL)
				t->current = t->current->right;
	}
}

void splice(tree t, treeDir dir, tree splicingOn)
{
	if(dir == left && t.current->left == NULL)
		t.current->left = splicingOn.root;
	if(dir == right && t.current->right == NULL)
		t.current->right = splicingOn.root;
}

void resetToRoot(tree *t)
{
	t->current = t->root;
}

void freeTree(tree t)
{
	while(t.root != NULL)
	{
		stepToLowestInternal(&t);
		free(t.current->data);
		free(t.current);
		if(t.current->parent->left == t.current)
		{
			t.current->parent->left = NULL;
		}
		else
		{
			t.current->parent->right = NULL;
		}
		resetToRoot(&t);
	}
}

void trim(tree *t, treeDir dir)
{
	tree toFree;
	toFree.dataSize = t->dataSize;
	switch(dir)
	{
		case left:
			toFree.root = t->current->left;
			t->current->left = NULL;
		case right:
			toFree.root = t->current->right;
			t->current->left = NULL;
		case up:
			toFree.root = t->root;
			if(t->current->parent->left == t->current)
				t->current->parent->left = NULL;
			else
				t->current->parent->right = NULL;
			t->current->parent = NULL;
			t->root = t->current;
	}
	freeTree(toFree);
}

treeNodeType currentNodeType(tree t)
{
	if(t.current->parent == NULL)
		return root;
	else if(t.current->left == NULL && t.current->right == NULL)
		return leaf;
	else if(t.current->left == NULL)
		return rightInternal;
	else if(t.current->right == NULL)
		return leftInternal;
	else
		return dualInternal;
}

void stepToLowestInternal(tree *t) /* TODO: this function */
{
	stack path = newStack(sizeof(treeDir));
	treeDir type;
	int deepest = 0;
	while(currentNodeType(*t) != leaf && currentNodeType(*t) != rightInternal)
	{
		type = left;
		step(t, type);
		spush(&type, &path);
		deepest++;
	}
}
