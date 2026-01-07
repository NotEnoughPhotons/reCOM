#include "zrender.h"
#include "gamez/zCamera/zcam.h"
#include "gamez/zNode/znode.h"
#include "gamez/zNode/node_world.h"
#include "gamez/zGrid/zgrid.h"
#include "gamez/zVisual/zvis.h"
#include "gamez/zVideo/zvid.h"

u32 numCharactersRendered = 0;
f32 bilinearDistance = 0.0f;
bool FirstVisDrawn = false;

void BuildNodeLocalLightList(zdb::CNode* node, zdb::CNode* light)
{
	// TODO: Implement this function.
	// It should gather up a list of local lights
	// and add them to the visual instances list.
}

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

		// Does this visual contain LODs?
		if (visual->m_useLOD)
		{
			CPnt3D position;

			zdb::CVisual::m_fogEnable = m_doFog;

			// Should fog affect the rendering of this visual?
			if (zdb::CVisual::m_fogEnable)
				zdb::CVisual::m_fogEnable = visual->m_doFog;

			if (visual->m_detail_cnt != 0)
			{
				// Transform the LOD mesh around the position of the visual instance.
				CStack::m_top->Transform(&visual->m_centroid, 1);

				// Determine the LOD mesh to use based upon distance from the camera.
				zdb::CVisual::m_rangeSqdToCamera = m_camera->GetScaledRangeSquared(position);
				zdb::CVisual::m_applyDetailTexture = false;

				for (u32 j = 0; j < visual->m_detail_cnt; j++)
					if (zdb::CVisual::m_rangeSqdToCamera <= visual->m_detail_buff[j].m_range_sqd_to_camera)
						zdb::CVisual::m_applyDetailTexture = true;
			}

			// Does this visual have bilinear filtering for textures?
			bool adjustBilinear = false;
			if (zdb::CVisual::m_adjustBilinearRange > 0.0f && visual->m_bilinear)
			{
				CStack::m_top->Transform(&visual->m_centroid, 1);
				
				// Use more detailed textures when the camera is up close.
				if (m_camera->GetScaledRangeSquared(position) < zdb::CVisual::m_adjustBilinearRange)
					adjustBilinear = true;
				else
				{
					// Camera is far away, so we'll need to use a texture with no bilinear filtering.
					u32 vidx = zdb::CVisual::m_stack_vid.size() - 1;
					
					_vdataTex->SetBilinear(visual->m_gifSelect, false);
				}
			}

			// Are there any visuals on the UI layer?
			if (m_doUI || visual->m_ui && opacity >= 1.0f && !adjustBilinear || m_simpletrans)
			{
				if (FirstVisDrawn)
				{
					FirstVisDrawn = false;
					// m_node->NodeSetup(node, mat, fov);

					if (zdb::CVisual::m_applyLocalLights)
						// Why do UI nodes need to have lights?
						BuildNodeLocalLightList(m_node, node);
				}

				// TODO: Fix this function. A matrix apparently isn't used or passed into
				// CVisual::VuUpdate. What???
				// Expected: visual->Render(mat);
				visual->Render();
				// CBench::countVisuals++;
			}
			else if (opacity >= 0.0078125f)
			{
				m_alpha.Add(m_node, node, visual, fov, opacity);
				// m_alpha.m_dyntexlist.Add(visual->m_dyntexList);
			}
		}
		else
		{
			CPnt3D pos(mat.m_matrix[0][3], mat.m_matrix[1][3], mat.m_matrix[2][3]);
			f32 scaledRange = m_camera->GetScaledRangeSquared(pos);

			u32 index = visual->m_useLOD;
			zdb::CLOD_band* lod = NULL;

			if (!index)
				lod = NULL;
			else
			{
				if (index <= m_world->m_LOD_Object.size())
					lod = m_world->m_LOD_Object[index];
			}

			if (!lod)
				visual->DrawLOD(lod, scaledRange, &opacity);

			if (m_LODFilter)
				return false;
		}
	}

	FirstVisDrawn = true;

	return true;
}

void CPipe::RenderUINode(zdb::CNode* node)
{

}

void CPipe::Flush()
{
	
}
