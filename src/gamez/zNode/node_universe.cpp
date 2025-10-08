#include "node_universe.h"

#define MIN_POOL_SIZE 3000
#define POOL_INCREMENT 100

namespace zdb
{
	CNodeUniverse* NodeUniverse = NULL;

	bool CNodeUniverse::AddNode(CNode* node)
	{
		bool success = false;

		if (!m_locked)
		{
			CNode* universeNode = node;
			int universeSize = size();

			if (universeSize == capacity())
			{
				if (capacity() < MIN_POOL_SIZE)
				{
					reserve(MIN_POOL_SIZE);
				}
				else
				{
					reserve(capacity() + POOL_INCREMENT);
				}
			}

			insert(begin(), universeNode);
			success = true;
		}
		else
		{
			success = false;
		}

		return success;
	}

	void CNodeUniverse::RemoveNode(CNode* node)
	{
		auto it = begin();
		while (it != end())
		{
			if (*it == node)
			{
				break;
			}

			it++;
		}

		*it = NULL;
	}

	CNode* CNodeUniverse::GetElement(s32 index) const
	{
		CNode* node;

		if (index < 0)
		{
			node = NULL;
		}
		else
		{
			node = NULL;

			if (index <= size())
			{
				node = at(index);
			}
		}

		return node;
	}

	int CNodeUniverse::GetIndex(CNode* node) const
	{
		int index = 0;
		auto it = begin();
		while (it != end())
		{
			if (*it == node)
			{
				break;
			}

			it++;
			index++;
		}

		return index;
	}
}