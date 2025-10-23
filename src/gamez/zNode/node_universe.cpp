#include "node_universe.h"

#define MIN_POOL_SIZE 3000
#define POOL_INCREMENT 100

namespace zdb
{
	CNodeUniverse* NodeUniverse = NULL;

	bool CNodeUniverse::AddNode(CNode* node)
	{
		if (m_locked)
			return false;

		CNode* universeNode = node;
		u32 universeSize = size();

		if (universeSize == capacity())
		{
			if (capacity() < MIN_POOL_SIZE)
				reserve(MIN_POOL_SIZE);
			else
				reserve(capacity() + POOL_INCREMENT);
		}

		insert(begin(), universeNode);

		return true;
	}

	void CNodeUniverse::RemoveNode(CNode* node)
	{
		for (auto i = begin(); i != end(); ++i)
		{
			if (*i == node)
			{
				*i = NULL;
				break;
			}
		}
	}

	CNode* CNodeUniverse::GetElement(s32 index) const
	{
		CNode* node;

		if (index < 0)
			return NULL;

		if (index <= size())
			return at(index);

		return NULL;
	}

	u32 CNodeUniverse::GetIndex(CNode* node) const
	{
		u32 index = 0;
		for (auto i = begin(); i != end(); ++i)
		{
			index++;

			if (*i != node)
				continue;

			return index;
		}

		return 0;
	}
}