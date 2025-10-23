#ifndef GRAPH_EDGE_H
#define GRAPH_EDGE_H
class CGEdge
{
public:
	CGEdge(CGNode* first, CGNode* second, f32 padding);

	bool Delete();
};
#endif