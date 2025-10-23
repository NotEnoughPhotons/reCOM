#ifndef GRAPH_NODE_H
#define GRAPH_NODE_H
class CGNode
{
public:
	CGNode(const char* name);
	~CGNode();

	CGEdge* FindEdge(CGNode* node);

	void SetName(const char* name);
};

class CGNodeEx : CGNode {};
#endif