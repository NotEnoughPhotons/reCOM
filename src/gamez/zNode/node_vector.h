#ifndef NODE_VECTOR_H
#define NODE_VECTOR_H
#include <vector>

namespace zdb
{
	class CRenderMap;

	class CRenderMapVector : public std::vector<CRenderMap*> {};
}
#endif