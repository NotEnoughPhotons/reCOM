#ifndef GRID_CLUTTER_H
#define GRID_CLUTTER_H
#include "gamez/zNode/znode.h"

namespace zdb
{
	class CClutter
	{
	private:
		CNode* m_node;
	};

	class CClutterList : public std::list<CClutter*>
	{

	};
}

#endif