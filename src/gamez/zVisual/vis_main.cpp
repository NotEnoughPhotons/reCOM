#include <GL/glew.h>

#include "zvis.h"

#include "gamez/zArchive/zar.h"

#include "gamez/zNode/znode.h"
#include "gamez/zNode/node_model.h"
#include "gamez/zNode/node_world.h"

#include "gamez/zRender/zrender.h"
#include "gamez/zShader/zshader.h"

s32 node_index = 0;
zdb::CVisData* _vdataTex = NULL;

void hookupMesh(zar::CZAR* archive, zdb::CModel* model)
{
	u32* data = NULL;
	_word128* meshword = NULL;
	char buf[192];
	
	zdb::CMesh* mesh = model->GetMesh();
	sprintf_s(buf, 192, "MESH_%s", model->m_name);

	if (auto meshkey = archive->OpenKey(buf))
	{
		if (archive->FetchLIP(meshkey, reinterpret_cast<void**>(meshword)))
		{
			u32 ofs = 0;
			
			strcat_s(buf, "_START");

			archive->Fetch(buf, &ofs);
			archive->Fetch(buf, &mesh->m_mtx_count);

			data = &meshword->u32[ofs];
		}

		archive->CloseKey(meshkey);
	}
}

void hookupVisuals(zar::CZAR* archive, zar::CKey* key, zdb::CNode* node, zdb::CModel* model, zdb::CVisBase* vis)
{
	char node_name[1024];
	char node_light_name[1024];

	u32 bufidx = 0;
	u32 vis_id, child_vis_id = 0;
	u32 node_ofs = 0;
	
	if (!node)
		return;

	if (node->m_hasMesh)
		node->GetSubMesh();

	if (!node->m_visual.empty())
	{
		node->m_hasVisuals = true;
		node_index++;
	}

	for (auto i = model->m_child.begin(); i != model->m_child.end(); i++)
	{
		zdb::CNode* n = *i;
		zdb::CNode* child = n->FindChild(node->m_name, true);

		if (!child)
			child = n;

		child->m_vid = vis_id;

		for (auto j = node->m_visual.begin(); j != node->m_visual.end(); j++)
		{
			zdb::CVisual* visual = *j;

			sprintf(node_name, "N%03d_I%03d_V%02d", node_index, vis_id, child_vis_id);
			strcpy(node_light_name, node_name);
			strcat(node_light_name, "L");

			if (archive->Fetch(node_light_name, &node_ofs, sizeof(u32)))
				child->SetDynamicLight(true, false);
			else if (archive->Fetch(node_name, &node_ofs, sizeof(u32)))
			{
				vis->m_node_ofs = node_ofs;
				visual->SetBuffer(&vis->m_data_buffer[vis->m_node_ofs], bufidx, vis);
			}
		}

		bufidx++;
	}

	if (&model->m_list == NULL || model->m_bForceExport)
	{
		zdb::CMesh* mesh = model->GetMesh();

		for (auto i = model->m_child.begin(); i != model->m_child.end(); i++)
		{
			zdb::CNode* n = *i;
			zdb::CNode* child = n->FindChild(node->m_name, true);

			if (!child)
				child = n;

			child->m_vid = vis_id;

			for (auto j = node->m_visual.begin(); j != node->m_visual.end(); j++)
			{
				zdb::CVisual* visual = *j;

				sprintf(node_name, "N%03d_I%03d_V%02d", node_index, vis_id, child_vis_id);
				strcpy(node_light_name, node_name);
				strcat(node_light_name, "L");

				if (archive->Fetch(node_light_name, &node_ofs, sizeof(u32)))
					child->SetDynamicLight(true, false);
				else if (archive->Fetch(node_name, &node_ofs, sizeof(u32)))
				{
					vis->m_node_ofs = node_ofs;
					visual->SetBuffer(&vis->m_data_buffer[vis->m_node_ofs], bufidx, vis);
				}
			}

			bufidx++;
		}
	}

	for (auto i = node->m_child.begin(); i != node->m_child.end(); i++)
	{
		zdb::CNode* parent = *i;
		zdb::CNode* child = NULL;

		if (parent->m_type == (u32)zdb::CNode::TYPE::NODE_TYPE_INSTANCE)
			child = parent;

		if (!child)
			hookupVisuals(archive, key, parent, model, vis);
	}
}

void hookupVisuals(zar::CZAR* archive, zdb::CModel* model)
{
	zdb::CVisBase* vis = NULL;

	if (model->GetMesh())
	{
		hookupMesh(archive, model);
		return;
	}

	node_index = -1;

	if (zar::CKey* key = archive->OpenKey(model->m_name))
	{
		vis = new zdb::CVisBase(key->GetSize());

		if (!archive->FetchLIP(key, reinterpret_cast<void**>(&vis)))
		{
			if (!vis->m_active)
			{
				if (vis->m_data_buffer)
					zfree(vis->m_data_buffer);

				vis->m_data_buffer = NULL;
			}

			zdb::CVisBase::m_instance_count--;
			delete vis;
		}
		else hookupVisuals(archive, key, model, model, vis);

		archive->CloseKey(key);
	}
}

namespace zdb
{
	u32 CVisBase::m_instance_count = 0;

	CPnt4D CVisual::m_basefog_color = CPnt4D::zero;
	
	CCamera* CVisual::m_camera = NULL;

	f32 CVisual::m_adjustBilinearRange = 0.0f;
	
	CVisBase::CVisBase(size_t size)
	{
		m_data_buffer = NULL;
		m_buffer_count = 0;
		m_data_size = size;
		m_instance_count++;
		m_active = true;
	}

	void CVisData::SetBilinear(_word128* qwc, bool bilinear)
	{
		while (m_nextGif->u8[2] != 8)
		{
			m_nextGif++;
		}
	}
	
	CVisual* CVisual::Create(zar::CZAR& archive)
	{
		CVisual* visual = NULL;
		u32 vtype = 0;

		archive.Fetch("vtype", &vtype);

		switch (vtype)
		{
		case 2:
			visual = new CSubMesh();
		case 1:
			visual = new CMesh();
		case 0:
		case -1:
			visual = new CVisual();
		}

		if (visual)
			visual->Read(archive);

		return visual;
	}

	bool CVisual::Read(zar::CZAR& archive)
	{
		u32 detail_size = 0;

		archive.Fetch("vparams", this, sizeof(tag_VIS_PARAMS));

		if (m_detail_buff == NULL)
			zfree(m_detail_buff);

		m_detail_buff = NULL;
		m_detail_cnt = 0;

		archive.Fetch("detail_cnt", &m_detail_cnt);
		
		if (!m_detail_cnt)
			return false;

		archive.Fetch("detail_size", &detail_size);
		m_detail_buff = static_cast<zdb::CVisual*>(zmalloc(detail_size));
		archive.Fetch("detail_buff", m_detail_buff, detail_size);

		for (u32 i = 0; i < m_detail_cnt; i++)
		{
			
		}

		return true;
	}

	bool CVisual::DrawLOD(zdb::CLOD_band* lod, f32 range, f32* distance)
	{
		if (lod->m_minRangeNearSq < range || lod->m_maxRangeFarSq > range)
			return false;

		if (!lod->m_minFade)
			return false;

		if (lod->m_minRangeFarSq < range || lod->m_maxRangeFarSq > range)
			return false;

		*distance *= lod->m_minInvDeltaRangeSq * (range - lod->m_minRangeNearSq);
		return true;
	}

	void CVisual::SetBuffer(_word128* wvis, u32 bufferidx, CVisBase* visdata)
	{
		visdata->m_buffer_count++;
		void* vis_buffer = visdata->m_data_buffer;
		m_dmaBuf->u128 = wvis->u128;

		u32 type = m_dmaBuf->u32[bufferidx];

		if (type == 0)
		{
			
		}
	}
	
	void CVisual::Render()
	{
		if (m_renderState != 0)
			VuUpdate(1.0f);
	}

	void CVisual::VuUpdate(f32 opacity)
	{

	}

	CTexture* CVisual::ResolveTextureName(_word128* wtexture, _word128* wname)
	{
		CTexture* texture = NULL;

		CTexHandle* handle = CWorld::GetTexHandle((char*)(wtexture->u8 + wname->u32[1]));

		if (!handle)
		{
			handle = CWorld::GetTexHandle("null_xmas.bmp");
		}

		if (handle)
		{
			texture = handle->m_texture;
			// m_dynTexList.Add(handle, true);

			if (texture->m_palette)
			{
				// m_dynTexList.Add(texture->m_palette, true);
			}
		}

		if (texture)
		{
			f32 r = texture->m_dmaRefVu.f32[1];
			f32 g = texture->m_dmaRefVu.f32[2];
			f32 b = texture->m_dmaRefVu.f32[3];
			wname->u128 = texture->m_dmaRefVu.u128;
			wname->f32[1] = r;
			wname->f32[2] = g;
			wname->f32[3] = b;
			wname->u8[2] = 5;
		}

		return texture;
	}
	
	u32 CVisual::Release()
	{
		m_instance_cnt--;
		return m_instance_cnt;
	}
	
	bool CVisualVector::Exists(const CVisual* visual)
	{
		for (auto it = begin(); it != end(); it++)
		{
			if (*it == visual)
				return true;
		}

		return false;
	}

	void CVisData::NextVertex(u32 stride)
	{
		
	}
}