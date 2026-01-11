#include <GL/glew.h>

#include "zvis.h"

#include "gamez/zArchive/zar.h"

#include "gamez/zAssetLib/assetlib_vector.h"

#include "gamez/zNode/znode.h"
#include "gamez/zNode/node_assetlib.h"
#include "gamez/zNode/node_model.h"
#include "gamez/zNode/node_world.h"

#include "gamez/zRender/zrndr_gl.h"
#include "gamez/zRender/zrender.h"
#include "gamez/zShader/zshader.h"

s32 node_index = 0;
zdb::CVisData* _vdataTex = NULL;
std::deque<u32> zdb::CVisual::m_stack_vid;
bool zdb::CVisual::m_applyShadow = true;
bool zdb::CVisual::m_applyDetailTexture = false;
bool zdb::CVisual::m_applyLocalLights = false;
bool zdb::CVisual::m_fogEnable = false;
bool zdb::CVisual::m_renderMap = false;
bool zdb::CVisual::m_landmarkFlag = false;
bool zdb::CVisual::m_lightingEnable = true;
f32 zdb::CVisual::m_rangeSqdToCamera = 1.0f;
_word128* zdb::CVisual::m_dmaChain = NULL;
u32 zdb::CVisual::m_dmaQwc = 0;

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

void hookupVisuals(zar::CZAR* archive, zar::CKey* key, zdb::CNode* node, zdb::CModel* model, zdb::CVisBase* vbase)
{
	char node_name[1024];
	char node_light_name[1024];

	u32 bufidx = 0;
	u32 vis_id = 0;
	u32 child_vis_id = 0;
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

	for (auto i = model->m_list.begin(); i != model->m_list.end(); i++)
	{
		zdb::CNode* n = static_cast<zdb::CModel*>((*i)->m_model_buffer);
		zdb::CNode* child = n->FindChild(node->m_name, true);

		if (!child)
			child = n;

		child->m_vid = vis_id;

		for (auto j = node->m_visual.begin(); j != node->m_visual.end(); j++)
		{
			zdb::CVisual* visual = *j;

			sprintf(node_name, "N%03d_I%03d_V%02d", node_index, vis_id, child_vis_id);
			strcpy(node_light_name, node_name);
			strcat(node_light_name, "_L");

			bool found = archive->Fetch(node_light_name, &node_ofs, sizeof(u32));

			if (!found)
				found = archive->Fetch(node_name, &node_ofs, sizeof(u32));
			else
				child->SetDynamicLight(true, false);

			if (found)
			{
				vbase->m_node_ofs = node_ofs;
				visual->SetBuffer(&vbase->m_data_buffer[vbase->m_node_ofs], bufidx, vbase);
			}
		}

		bufidx++;
	}

	if (model->m_list.size() == 0 || model->m_bForceExport)
	{
		zdb::CMesh* mesh = model->GetMesh();

		for (u32 i = 0; i != node->m_visual.size(); i++)
		{
			zdb::CVisual* visual = node->m_visual[i];

			sprintf(node_name, "N%03d_I%03d_V%02d", node_index, 0, i);
			strcpy(node_light_name, node_name);
			strcat(node_light_name, "_L");

			bool found = archive->Fetch(node_light_name, &node_ofs, sizeof(u32));

			if (!found)
				found = archive->Fetch(node_name, &node_ofs, sizeof(u32));
			else
				node->SetDynamicLight(true, false);

			if (!found)
			{
				sprintf(node_name, "N%03d_%03d", node_index, i);
				strcpy(node_light_name, node_name);
				strcat(node_light_name, "_L");

				if (!found)
					found = archive->Fetch(node_name, &node_ofs, sizeof(u32));
				else
					node->SetDynamicLight(true, false);

				if (found)
				{
					vbase->m_node_ofs = node_ofs;
					visual->m_chainPtr = (_word128*)((u8*)vbase->m_data_buffer + vbase->m_node_ofs);
					visual->SetBuffer(visual->m_chainPtr, 0, vbase);
				}
			}
		}
	}

	for (auto i = node->m_child.begin(); i != node->m_child.end(); i++)
	{
		zdb::CNode* parent = *i;
		zdb::CNode* child = NULL;

		if (parent->m_type == (u32)zdb::CNode::TYPE::NODE_TYPE_INSTANCE)
			child = parent;

		if (!child)
			hookupVisuals(archive, key, parent, model, vbase);
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

		if (!archive->FetchLIP(key, reinterpret_cast<void**>(vis)))
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
		else 
			hookupVisuals(archive, key, model, model, vis);

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

	void CVisual::LandmarkEnable(bool enable)
	{
		m_landmarkFlag = enable;
		m_lightingEnable = !enable;
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

	void CVisual::SetBuffer(_word128* chain, u32 idx, CVisBase* vbase)
	{
		vbase->m_buffer_count++;
		_word128* buffer = vbase->m_data_buffer;
		m_chainPtr[idx] = *chain;
		_word128* packet = &m_chainPtr[idx];

		// Check a flag? Maybe if the chain has its buffer set?
		if (packet->u8[2] != 0)
			return;

		// Store pointer to visual data
		packet->u32[2] = reinterpret_cast<u32>(&vbase);

		// Number of quadwords to loop over
		u8 qwc = packet->u8[0];

		for (u32 i = 1; i < qwc + 1; i++)
		{
			_word128& chainPkt = m_chainPtr[i];
			u8 type = chainPkt.u8[2];

			u32 pktOffset = chainPkt.u32[1];
			u32 address = reinterpret_cast<u32>(&*buffer);

			switch (type)
			{
			case 1:
			case 2:
			case 4:
			case 7:
				chainPkt.u32[1] = address + pktOffset;
				break;
			case 3:
				break;
			case 5:
				break;
			case 6:
				CTexture* texture = ResolveTextureName(&*buffer, pktOffset);

				// TODO: do some DMA ref BS down here
				if (texture)
				{
					
				}
				break;
			}
		}
	}
	
	bool CVisual::GetChainData()
	{
		u32 stackidx = m_stack_vid.size() - 1;
		_word128* tag = &m_chainPtr[m_stack_vid[stackidx % m_stack_vid.size()]];
		m_dmaChain = &tag[1];
		m_dmaQwc = *tag[0].u8;

		return true;
	}

	void CVisual::Render()
	{
		// if (m_renderState != 0)
			VuUpdate(1.0f);
	}

	void CVisual::VuUpdate(f32 opacity)
	{
		GetChainData();
	}

	CTexture* CVisual::ResolveTextureName(_word128* packet, s32 offset)
	{
		if (CTexHandle* handle = CWorld::GetTexHandle(reinterpret_cast<char*>(packet->u8 + offset)))
		{
			m_dyntexList.Add(handle, true);

			if (handle->m_texture->m_palette)
				m_dyntexList.Add(handle->m_texture->m_palette, true);

			return handle->m_texture;
		}
		else
			return CWorld::GetTexHandle("null_xmas.bmp")->m_texture;

		return NULL;
	}

	CTexture* CVisual::ResolveTextureName(_word128* packet, _word128* offset)
	{
		CTexture* texture = NULL;

		CTexHandle* handle = CWorld::GetTexHandle((char*)(packet->u8 + offset->u32[1]));

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
			offset->u128 = texture->m_dmaRefVu.u128;
			offset->f32[1] = r;
			offset->f32[2] = g;
			offset->f32[3] = b;
			offset->u8[2] = 5;
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