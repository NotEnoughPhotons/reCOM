#include "node_world.h"
#include "node_assetlib.h"

#include "gamez/zAssetLib/zassetlib.h"
#include "gamez/zCamera/zcam.h"
#include "gamez/zGrid/zgrid.h"
#include "gamez/zVisual/zvis.h"
#include "gamez/zTexture/ztex.h"

namespace zdb
{
	CWorld* CWorld::m_world = NULL;
	f32 CWorld::m_scale = 1.0f;
	f32 CWorld::m_invscale = 1.0f / CWorld::m_scale;

	CWorld::CWorld(const char* name)
	{
		m_camera = NULL;
		m_diInt = NULL;
		m_Wind.Reset();

		m_cellData = NULL;

		m_maxOverlap = 2;

		m_grid = new CGrid();

		SetName(name);

		m_scale = 10.0f;
		m_invscale = 0.1f;
		m_world = this;

		m_diInt = new DiIntersect(1024);
		m_diIntDelayCnt = 0.0f;

		m_gLightDirUpdate = true;
		m_gLightColUpdate = true;

		m_default_soiltype = 0;
		m_default_soiltype_name = NULL;
	}
	
	s32 CWorld::GetVersion()
	{
		return 0x10001;
	}

	CModel* CWorld::GetModel(const char* name)
	{
		return CAssetMgr::m_assets.GetModel(name);
	}

	bool CWorld::Initalize()
	{
		CCamera* camera = new CCamera();

		if (m_camera != camera)
		{
			m_camera = camera;
			CVisual::m_camera = camera;
		}

		m_camera->SetName("camera");
		m_camera->SetPosition(CPnt3D::zero.x, CPnt3D::zero.y, CPnt3D::zero.z);

		m_world->m_gLightColUpdate = true;
		m_world->m_gLightDirUpdate = true;

		// CTexHandle* handle = CTexHandle::Create(m_shadowTex[0]);
		// m_shadowTexH = handle;

		// CTexManager::m_texmanager->front()->Add(handle, true);

		return true;
	}

	void CWorld::ReserveChildren(s32 count)
	{
		m_child.reserve(count + m_child.capacity());
	}

	void CWorld::AddChild(CNode* child)
	{
		CWorld* parent = static_cast<CWorld*>(child->m_parent);

		if (parent != this)
		{
			CNode* node = child;

			if (parent != NULL && parent != this)
			{
				parent->DeleteChild(node);
			}

			// m_grid->Insert(node);

			node->m_parent = static_cast<CNode*>(this);

			if (m_child.capacity() == m_child.size())
			{
				m_child.reserve(m_child.capacity() + 100);
			}

			m_child.insert(m_child.begin(), node);

			CNode* temp = node;
			if (node->m_type != 2)
			{
				temp = NULL;
			}

			if (temp != NULL)
			{

			}

			// TODO:
			// Finish the part of this function that inserts "render maps", so the world can
			// dispatch them to the zRenderer.
		}
	}

	void CWorld::AddLandmark(CNode* landmark)
	{
		if (landmark->m_landmark)
		{

		}

		// landmark->RemoveFromParent();
	}

	bool CWorld::DismemberWorldModel()
	{
		CModel* worldmodel = CAssetMgr::m_assets.GetModel("worldmodel");

		if (!worldmodel)
		{
			return false;
		}

		// ReserveChildren(worldmodel->m_reflist.size());

		auto it = worldmodel->m_child.begin();

		while (it != worldmodel->m_child.end())
		{
			CNode* child = *it;
			m_child.insert(m_child.begin(), child);
			child->m_parent = this;
			// m_grid->Update();
			++it;
		}

		return true;
	}

	void CWorld::GarbageCollect()
	{
		for (auto i = m_child.begin(); i != m_child.end(); ++i)
		{
			zdb::CNode* node = dynamic_cast<CCell*>(*i);

			if (!node)
				(*i)->ShrinkAtomBasePtrs();
		}
	}

	CLightList& CWorld::GenerateLightList()
	{
		LightList_Search(this, m_lights);

		for (auto i = m_lights.begin(); i != m_lights.end(); ++i)
		{
			CNode* light = *i;

			CPnt3D worldPos;
			CPnt3D localPos;
			CPnt3D gridPos;

			light->TransformToWorld(worldPos);

			light->m_matrix.m_matrix[2][3] = worldPos.x;
			light->m_matrix.m_matrix[3][0] = worldPos.y;
			light->m_matrix.m_matrix[3][1] = worldPos.z;

			localPos.y = 0.0f;
			localPos.x = worldPos.x - light->m_matrix.m_matrix[0][2];
			localPos.z = worldPos.z - light->m_matrix.m_matrix[0][2];

			gridPos.x = worldPos.x + light->m_matrix.m_matrix[0][2];
			gridPos.y = 0.0f;
			gridPos.z = worldPos.z + light->m_matrix.m_matrix[0][2];

			m_grid->SetTraversalBoundary(&localPos, 2, false);

			for (CGridAtom* atom = m_grid->StartTraversal(); atom != NULL; atom = m_grid->GetNextAtom())
			{
				CNode* cell = atom->GetCell();

				if (!cell)
					continue;

				cell->m_child.insert(cell->m_child.begin(), light);
			}
		}

		return m_lights;
	}

	s32 CWorld::GenerateLandmarkList()
	{
		CNodeVector landmarks;
		CStack stack;
		
		CStack::m_pointer++;
		CStack::m_top++;

		// *CStack::m_top = CMatrix::identity;

		m_numNoFarClipNodes = 0;
		LandmarkList_Search(this, stack, landmarks);

		auto it = landmarks.begin();
		while (it != landmarks.end())
		{
			AddChild(*it);
			++it;
		}
		
		return 1;
	}

	CTexHandle* CWorld::GetTexHandle(char* name)
	{
		return CAssetMgr::m_assets.GetTexHandle(name);
	}
	
	void CWorld::LandmarkList_Search(CNode* node, CStack& matstack, CNodeVector& landmarks)
	{
		
	}
}