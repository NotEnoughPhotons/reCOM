#ifndef GRAPH_GRAPH_H
#define GRAPH_GRAPH_H
#include "graph_node.h"
#include "graph_edge.h"
#include "graph_tree.h"

#include "gamez/zUtil/util_systemio.h"

class CGraph
{
public:
	CGraph(const char* name);
	~CGraph();

	CGraph* Create(const char* name);
	CGraph* Create(CFileIO& io,
		void(*)(CGraph*, CGNode*, bool, u32&, void*),
		void(*)(CGraph*, CGEdge*, bool, u32&, void*));
	void Destroy();

	CGraph* Read(CFileIO& io,
		void(*)(CGraph*, CGNode*, bool, u32&, void*),
		void(*)(CGraph*, CGEdge*, bool, u32&, void*));

	void PopulateTree(CGTree* tree);

	CGEdge* CreateEdge(CGNode* first, CGNode* second);
	CGEdge* FindEdge(CGNode* first, CGNode* second);

	CGNode* CreateNode(const char* name);
	void DestroyNode(CGNode* node);
	CGNode* GetNodeByIndex(u32 index);
	CGNode* GetNodeByNodeEx(const CGNodeEx* node);
};
#endif