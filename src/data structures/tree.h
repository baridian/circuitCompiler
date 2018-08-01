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
    int dataSize;
    treeNode *current;
} tree;

typedef enum treeDir{left, right, up} treeDir;

tree newTree(int);

void addNode(tree,treeDir,void *);

void step(tree *,treeDir);

void splice(tree,treeDir,tree);

void trim(tree,treeDir);

void stepToLowest(tree *);