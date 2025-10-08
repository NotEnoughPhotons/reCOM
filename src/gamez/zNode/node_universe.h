#ifndef NODE_UNIVERSE_H
#define NODE_UNIVERSE_H
#include <vector>

#include "znode.h"

namespace zdb
{
	class CNodeUniverse : public std::vector<CNode*>
	{
	public:
		bool AddNode(CNode* node);
		void RemoveNode(CNode* node);
		CNode* GetElement(s32 index) const;
		s32 GetIndex(CNode* node) const;
	public:
		bool m_locked;
	};
}
#endif