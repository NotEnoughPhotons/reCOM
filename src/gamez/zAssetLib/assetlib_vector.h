#ifndef ASSETLIB_VECTOR_H
#define ASSETLIB_VECTOR_H
#include <list>

namespace zdb
{
	class CAssetLib;
}

class CRefList : public std::list<zdb::CAssetLib*> {};
#endif