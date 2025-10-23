#ifndef GRAPH_ROUTE_H
#define GRAPH_ROUTE_H
#include "graph_node.h"

class CGRoute
{
public:
	CGRoute();
	~CGRoute();

	void Append(CGNode* node);
	void PostProcess();
	void Clear();
};
#endif