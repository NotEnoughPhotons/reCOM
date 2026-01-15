#include "zrender.h"
#include "gamez/zCamera/zcam.h"
#include "gamez/zNode/znode.h"
#include "gamez/zNode/node_shadow.h"
#include "gamez/zNode/node_world.h"
#include "gamez/zGrid/zgrid.h"
#include "gamez/zSystem/zsys_bench.h"
#include "gamez/zMath/zmath_vu.h"
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

void CPipe::RenderNode(zdb::CNode* node, zdb::tag_ZVIS_FOV fov)
{
	bool doAlpha = false;

	if (!node->m_active)
		return;

	if (!node->m_hasVisuals)
		return;

	if (!m_camera->CanSeeRegion(node->m_region_mask))
		return;

	CBench::countNodes++;

	CMatrix nodeTransform = *stack.Multiply(&node->m_matrix, true);

	// Take care of any nodes in the frustum
	if (fov != zdb::tag_ZVIS_FOV::ZVIS_FOV_ALL_IN)
	{
		CBBox* bbox = NULL;

		PNT4D bboxtransform;
		PNT3D bboxmin;
		PNT3D bboxmax;

		PNT4D mx = reinterpret_cast<PNT4D&>(nodeTransform.m_matrix[0]);
		PNT4D my = reinterpret_cast<PNT4D&>(nodeTransform.m_matrix[1]);
		PNT4D mz = reinterpret_cast<PNT4D&>(nodeTransform.m_matrix[2]);
		PNT4D mw = reinterpret_cast<PNT4D&>(nodeTransform.m_matrix[3]);

		if (node->m_use_parent_bbox)
			bbox = node->m_parent->GetBBox();
		else
			bbox = &node->m_bbox;

		bboxmin = bbox->m_min;
		bboxmax = bbox->m_max;

		bboxtransform.w = 1.0f;
		bboxtransform.x = bboxmin.x;
		bboxtransform.y = bboxmin.y;
		bboxtransform.z = bboxmax.z;

		// TODO: do bbox transform math here
		for (u32 i = 8; i > 0; i--)
		{

		}
	}

	if (node->m_mtx_is_identity && !m_drawCharacters)
	{
		node->m_mtx_is_identity = false;
		CStack::m_pointer--;
		CStack::m_top--;
	}
	else
	{
		zdb::CVisual::m_stack_vid.push_back(node->m_vid);

		// Add node to opacity stack
		if (node->m_Opacity < 0.99f)
		{
			m_opacity_stack++;
			if (m_opacity_stack_index == m_opacity_stack_size)
			{
				// Extend stack
				m_opacity_stack = static_cast<f32*>(zrealloc(m_opacity_stack, (m_opacity_stack_size + 1) * 4));
				m_opacity_stack_size++;
			}

			doAlpha = true;
			m_opacity_stack[m_opacity_stack_index] *= node->m_Opacity;
		}

		if (node->m_facade)
			// Compute sprite billboard for this node
			node->ComputeFacadeMatrix(&m_camera->m_matrix, &nodeTransform);

		if (node->m_visual.size() != 0)
		{
			if (!m_LODFilter)
				RenderVisual(node, fov);
			else if (!RenderVisual(node, fov))
			{
				CStack::m_pointer--;
				CStack::m_top--;

				if (doAlpha)
					m_opacity_stack_index--;

				zdb::CVisual::m_stack_vid.pop_back();
				return;
			}
		}

		for (auto i = node->m_child.begin(); i != node->m_child.end(); ++i)
		{
			zdb::CNode* child = *i;

			if (!child->m_hasMesh)
				RenderNode(child, fov);
			else
				RenderMesh(child, NULL, NULL, zdb::tag_ZVIS_FOV::ZVIS_FOV_CLIP);
		}

		CStack::m_pointer--;
		CStack::m_top--;

		if (doAlpha)
			m_opacity_stack_index--;

		zdb::CVisual::m_stack_vid.pop_back();
	}
}

u32 CPipe::RenderWorld(zdb::CWorld* world)
{
	numCharactersRendered = 0;
	
	// TODO: get register value REG_RCNT0_COUNT
	// and substitute here
	CBench::drawTimeX = zSys.timerScale;

	m_texLoadIdx = 0;
	m_texIntIdx = 0;

	zVid_ClearColor(world->m_camera->m_fog_color.x, world->m_camera->m_fog_color.y, world->m_camera->m_fog_color.z);

	zdb::CDecal::m_tempPool.RecycleTick();
	zdb::CMeshDecal::m_tempPool.RecycleTick();

	m_polys = false;
	m_world = world;

	CStack::m_top++;
	CStack::m_pointer++;
	*CStack::m_top = CMatrix::identity;
	zMathCopyMatrix(CStack::m_top, &CMatrix::identity);

	zdb::CVisual::m_stack_vid.push_back(0);

	world->Update();
	zdb::CVisual::AlphaEnable(false);
	m_drawCharacters = true;

	if (world->m_shadows.size() != 0)
	{
		zdb::CVisual::m_applyShadow = false;
		zdb::CVisual::m_renderMap = true;
		zdb::CVisual::m_lightingEnable = false;
		zdb::CVisual::m_fogEnable = false;

		for (auto i = world->m_shadows.begin(); i != world->m_shadows.end(); ++i)
		{
			zdb::CRenderMap* shadow = *i;

			m_node = shadow;
			shadow->Update();

			// if (!shadow->m_hasMesh)
				// RenderNodeShadow();
			// else
				// RenderMeshShadow();
		}

		zdb::CVisual::m_renderMap = false;
		zVid_FrameRestore();
	}

	m_alpha.m_camera = m_camera;
	m_drawCharacters = false;

	zdb::CVisual::m_adjustBilinearRange = bilinearDistance;

	if (world->m_landmarks.size() != 0)
	{
		zdb::CVisual::LandmarkEnable(true);

		for (auto i = world->m_landmarks.begin(); i != world->m_landmarks.end(); ++i)
		{
			zdb::CNode* landmark = *i;

			if (landmark->m_landmark)
			{
				f32 radius = landmark->GetRadius();
				m_doFog = m_camera->TestLandmarkFOG((CPnt3D*)(landmark->m_matrix.m_matrix[3]), radius);
			}

			RenderNode(m_node, zdb::tag_ZVIS_FOV::ZVIS_FOV_CLIP);
		}

		zdb::CVisual::LandmarkEnable(false);
	}

	for (auto i = world->m_child.begin(); i != world->m_child.end(); ++i)
	{
		zdb::CNode* child = *i;
		child->SetActive(true);
		RenderNode(child, zdb::tag_ZVIS_FOV::ZVIS_FOV_ALL_IN);
	}

	m_drawCharacters = true;

	CStack::m_pointer--;
	CStack::m_top--;

	zdb::CVisual::m_stack_vid.pop_back();

	return true;
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
		if (!visual->m_lodIndex)
		{
			CPnt3D position;

			zdb::CVisual::m_fogEnable = m_doFog;

			// Should fog affect the rendering of this visual?
			if (zdb::CVisual::m_fogEnable)
				zdb::CVisual::m_fogEnable = visual->m_doFog;

			if (visual->m_detail_cnt != 0)
			{
				// Transform the visual around the center..
				CStack::m_top->Transform(&visual->m_centroid, 1);

				// Determine the visual to use based upon distance from the camera.
				zdb::CVisual::m_rangeSqdToCamera = m_camera->GetScaledRangeSquared(position);
				zdb::CVisual::m_applyDetailTexture = false;

				//for (u32 j = 0; j < visual->m_detail_cnt; j++)
				//	if (zdb::CVisual::m_rangeSqdToCamera <= visual->m_detail_buff[j].m_range_sqd_to_camera)
				//		zdb::CVisual::m_applyDetailTexture = true;
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
					zdb::CVisual::NodeSetup(m_node, node, &mat, static_cast<s32>(fov));

					if (zdb::CVisual::m_applyLocalLights)
						// Why do UI nodes need to have lights?
						BuildNodeLocalLightList(m_node, node);
				}

				// TODO: Fix this function. A matrix apparently isn't used or passed into
				// CVisual::VuUpdate. What???
				// Expected: visual->Render(mat);
				visual->Render(&mat);
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

			u32 index = visual->m_lodIndex;
			zdb::CLOD_band* lod = NULL;

			if (!index)
				lod = NULL;
			else
			{
				//if (index <= m_world->m_LOD_Object.size())
					//lod = m_world->m_LOD_Object[index];
			}

			if (!lod || visual->DrawLOD(lod, scaledRange, &opacity))
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
	zMathCopyMatrix(CStack::m_top, &CMatrix::identity);
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
