#include "zrender.h"
#include "gamez/zNode/znode.h"
#include "gamez/zNode/node_world.h"
#include "gamez/zGrid/zgrid.h"
#include "gamez/zVideo/zvid.h"

u32 numCharactersRendered = 0;
f32 bilinearDistance = 0.0f;

u32 CPipe::RenderWorld(zdb::CWorld* world)
{
	numCharactersRendered = 0;
	
	zVid_ClearColor(120.0f, 0.0f, 0.0f);

	// zdb::CDecal::m_tempPool.RecycleTick();
	// zdb::CMeshDecal::m_tempPool.RecycleTick();

	m_polys = false;
	m_world = world;

	// CStack::m_top++;
	// CStack::m_pointer++;
	// *CStack::m_top = CMatrix::identity;

	world->Update();
	zdb::CVisual::AlphaEnable(false);

	m_drawCharacters = false;

	m_alpha.m_camera = m_camera;
	m_drawCharacters = false;

	zdb::CVisual::m_adjustBilinearRange = bilinearDistance;

	zdb::CGridAtom* atom = world->m_grid->StartTraversalOrdered();
	
	return 1;
}

void CPipe::RenderUINode(zdb::CNode* node)
{

}

void CPipe::Flush()
{
	
}
