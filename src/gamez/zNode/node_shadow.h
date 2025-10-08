#ifndef NODE_SHADOW_H
#define NODE_SHADOW_H
#include "node_light.h"

namespace zdb
{
	// TODO:
	// figure out what is in this unknown struct
	// it is taken as a parameter in the constructor of
	// CRenderMap::CRenderMap(CNode*, tag_render_map_type, CTexture*)
	struct tag_render_map_type
	{

	};

	class CRenderMap : public CLightMap
	{
	public:
		CRenderMap(CNode*, tag_render_map_type, CTexture*);

		void Update();
	};
}
#endif