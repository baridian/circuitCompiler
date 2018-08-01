#include "stack.h"

#ifndef CRCT_TREE_H
#define CRCT_TREE_H

#endif //CRCT_TREE_H

typedef struct treeNode treeNode;

struct treeNode
{
	treeNode *left;
	treeNode *right;
	treeNode *parent;
	void *data;
};

typedef struct tree
{
	treeNode *root;
	unsigned dataSize;
	treeNode *current;
} tree;

typedef enum treeDir
{
	left, right, up
} treeDir;

typedef enum treeNodeType
{
	root, dualInternal, leftInternal, rightInternal, leaf
} treeNodeType;

tree newTree(int);

void addNode(tree, treeDir, void *);

void step(tree *, treeDir);

void splice(tree, treeDir, tree);

void trim(tree *, treeDir);

treeNodeType currentNodeType(tree);

int isRoot(tree);

static void stepLeftWhilePossible(tree *, int *, int *, treeNode **, stack *);

static void stepUpToFirstDual(tree *, stack, int *);

static void stepRightIfDual(tree *, int *, stack *);

static void stepRightUntilCanStepLeft(tree *, int *, int *, treeNode **);

static void stepUpAndIntoFirstUnvisitedRightNodeIfLeaf(tree *, stack *, int *);

void stepToLowestInternal(tree *);

void resetToRoot(tree *);

void freeTree(tree);
