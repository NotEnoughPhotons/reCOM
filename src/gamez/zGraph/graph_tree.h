#ifndef GRAPH_TREE_H
#define GRAPH_TREE_H
#include "graph_node.h"

class CGTree
{
public:
	CGNode* FindNode(CGNode* node);
	void RemoveChild(CGTree* tree);
};
#endif