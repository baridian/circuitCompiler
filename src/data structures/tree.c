#include <stdlib.h>
#include "tree.h"
/*TODO: add all function implementations*/
/*TODO: start work on symbol parser??? */

tree newTree(int dataSize)
{
    tree toReturn;
    toReturn.root = NULL;
    toReturn.dataSize = (unsigned)dataSize;
    toReturn.current = NULL;
    return toReturn;
}

void addNode(tree,treeDir,void *);

void step(tree *,treeDir);

void splice(tree,treeDir,tree);

void trim(tree,treeDir);

void stepToLowestInternal(tree *);

void freeTree(tree);