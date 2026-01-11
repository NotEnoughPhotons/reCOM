#ifndef VIS_VECTOR_H
#define VIS_VECTOR_H
#include <vector>

namespace zdb
{
	class CVisual;
	class CDecal;
	class CMeshDecal;

	class CVisualVector : public std::vector<CVisual*>
	{
	public:
		bool Exists(const CVisual* visual);
	};

	// TODO: Implement and use RTArray<T> as base class
	// A vector will be used for now
	class CDecalPool : public std::vector<CDecal*>
	{
	public:
		void RecycleTick();
	};

	class CMeshDecalPool : public std::vector<CMeshDecal*>
	{
	public:
		void RecycleTick();
	};
}
#endif