#include "zrender.h"
#include "gamez/zCamera/zcam.h"
#include "gamez/zNode/znode.h"
#include "gamez/zNode/node_world.h"
#include "gamez/zGrid/zgrid.h"
#include "gamez/zVisual/zvis.h"
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

bool CPipe::RenderVisual(zdb::CNode* node, zdb::tag_ZVIS_FOV fov)
{
	CMatrix mat = *CStack::m_top;
	// CBench::countRenderNodes++;

	f32 opacity = m_opacity_stack[m_opacity_stack_index];

	for (u32 i = 0; i < node->m_visual.size(); i++)
	{
		zdb::CVisual* visual = node->m_visual[i];

		if (visual->m_useLOD)
		{
			CPnt3D position;

			zdb::CVisual::m_fogEnable = m_doFog;

			if (zdb::CVisual::m_fogEnable)
				zdb::CVisual::m_fogEnable = visual->m_doFog;

			if (visual->m_detail_cnt != 0)
			{
				CStack::m_top->Transform(&visual->m_centroid, 1);

				zdb::CVisual::m_rangeSqdToCamera = m_camera->GetScaledRangeSquared(position);
				zdb::CVisual::m_applyDetailTexture = false;

				for (u32 j = 0; j < visual->m_detail_cnt; j++)
					if (zdb::CVisual::m_rangeSqdToCamera <= visual->m_detail_buff[j].m_range_sqd_to_camera)
						zdb::CVisual::m_applyDetailTexture = true;
			}
		}
	}

	return false;
}

void CPipe::RenderUINode(zdb::CNode* node)
{

}

void CPipe::Flush()
{
	
}
