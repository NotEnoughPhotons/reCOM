#include "zrender.h"
#include "gamez/zCamera/zcam.h"
#include "gamez/zNode/znode.h"
#include "gamez/zNode/node_world.h"
#include "gamez/zGrid/zgrid.h"
#include "gamez/zSystem/zsys_bench.h"
#include "gamez/zVisual/zvis.h"
#include "gamez/zVideo/zvid.h"

u32 numCharactersRendered = 0;
f32 bilinearDistance = 0.0f;
bool FirstVisDrawn = false;
CStack stack;

void BuildNodeLocalLightList(zdb::CNode* node, zdb::CNode* light)
{
	// TODO: Implement this function.
	// It should gather up a list of local lights
	// and add them to the visual instances list.
}

void CPipe::RenderAtom(zdb::CNode* atomNode)
{
	m_node = atomNode;
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
	CBench::countRenderNodes++;

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
				CBench::countVisuals++;
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

void CPipe::RenderUiNode(zdb::CNode* node)
{
	CStack::m_top++;
	CStack::m_pointer++;
	// vu0CopyMatrix(CStack::m_top->m_matrix, CMatrix::identity.m_matrix);
	RenderUiNodeRecursive(node);
	CStack::m_pointer--;
	CStack::m_top--;
}

void CPipe::RenderUiNodeRecursive(zdb::CNode* node)
{
	bool doAlpha = false;

	if (node->m_active)
		return;

	CBench::countNodes++;

	zdb::CVisual::m_stack_vid.push_back(node->m_vid);
	stack.Multiply(&node->m_matrix, true);

	if (node->m_Opacity < 0.99f)
	{
		m_opacity_stack_index++;
		if (m_opacity_stack_index == m_opacity_stack_size)
		{
			m_opacity_stack = static_cast<f32*>(zrealloc(m_opacity_stack, (m_opacity_stack_size + 1) * 4));
			m_opacity_stack_size++;
		}

		doAlpha = true;

		m_opacity_stack[m_opacity_stack_index] = node->m_Opacity * m_opacity_stack[m_opacity_stack_index - 1];
	}

	if (node->m_visual.size())
		RenderVisual(node, zdb::tag_ZVIS_FOV::ZVIS_FOV_CLIP);

	for (auto i = node->m_child.begin(); i != node->m_child.end(); ++i)
	{
		zdb::CNode* node = *i;
		if (!node->m_hasMesh)
			RenderUiNodeRecursive(node);
		else
			RenderMesh(node, NULL, NULL, zdb::tag_ZVIS_FOV::ZVIS_FOV_CLIP);
	}

	if (node)
	{
		CStack::m_pointer--;
		CStack::m_top--;
	}

	if (doAlpha)
	{
		m_opacity_stack_index--;
	}

	zdb::CVisual::m_stack_vid.pop_back();
}

void CPipe::Flush()
{
	
}
