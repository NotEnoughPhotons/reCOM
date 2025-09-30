#include <GL/glew.h>

#include "zvis.h"

#include "gamez/zArchive/zar.h"
#include "gamez/zNode/znode.h"
#include "gamez/zRender/zrender.h"
#include "gamez/zShader/zshader.h"

s32 node_index = 0;

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
	
	if (!node)
		return;

	if (node->m_hasMesh)
		node->GetSubMesh();

	if (!node->m_visual.empty())
	{
		node->m_hasVisuals = true;
		node_index++;
	}

	u32 vis_id = 0;
	u32 child_vis_id = 0;
	
	for (auto i = model->m_child.begin(); i != model->m_child.end(); i++)
	{
		zdb::CNode* n = *i;
		zdb::CNode* child = n->FindChild(node->m_name, true);

		if (!child)
			child = n;

		child->m_vid = vis_id;

		for (auto j = child->m_visual.begin(); j != child->m_visual.end(); j++)
		{
			zdb::CVisual* visual = *j;

			sprintf(node_name, "N%03d_I%03d_V%02d", node_index, vis_id, child_vis_id);
			strcpy(node_light_name, node_name);
			strcat(node_light_name, "L");
		}
	}

	auto node_iterator = model->m_child.begin();
	while (node_iterator != model->m_child.end())
	{
		zdb::CNode* node = *node_iterator;
		zdb::CNode* child = node->FindChild(node->m_name, true);

		if (!child)
		{
			child = node;
		}

		child->m_vid = vis_id;

		auto vis_iterator = child->m_visual.begin();
		while (vis_iterator != child->m_visual.end())
		{
			zdb::CVisual* visual = *vis_iterator;

			// TODO: Figure out what the visual vectors are storing
			// visual->some_vector_1.insert(visual->some_vector_1.end(), lod);
			// visual->some_vector_2.insert(visual->some_vector_2.end(), something);

			sprintf(node_name, "N%03d_I%03d_V%02d", node_index, vis_id, child_vis_id);
			strcpy(node_light_name, node_name);
			strcat(node_light_name, "L");

			u32 local24 = 0;
			bool fetched = false;

			fetched = archive->Fetch(node_light_name, &local24, sizeof(u32));
			
			if (!fetched)
			{
				fetched = archive->Fetch(node_name, &local24, sizeof(u32));
			}
			else
			{
				child->SetDynamicLight(true, false);
			}

			if (!fetched)
			{
				// TODO: More visual vector stuff needs to be figured out -
				// And implemented!
			}
			else
			{
				// TODO: Add quadword parameters to this function -
				// to target a MultiGen vertex buffer
				// visual->SetBuffer();
			}
			
			++vis_iterator;
		}
		
		++node_iterator;
	}
}

void hookupVisuals(zar::CZAR* archive, zdb::CModel* model)
{
	zdb::CVisBase* vis = NULL;
	zdb::CMesh* mesh = model->GetMesh();

	if (!mesh)
	{
		node_index = -1;

		zar::CKey* modelkey = archive->OpenKey(model->m_name);

		if (modelkey)
		{
			vis = new zdb::CVisBase(modelkey->GetSize());
			
			if (!archive->FetchLIP(modelkey, reinterpret_cast<void**>(vis)))
			{
				if (modelkey && !vis->m_active)
				{
					if (vis->m_data_buffer)
					{
						zfree(vis->m_data_buffer);
					}

					vis->m_data_buffer = NULL;
				}

				zdb::CVisBase::m_instance_count--;
				delete vis;
			}
			else
			{
				hookupVisuals(archive, modelkey, model, model, vis);
			}

			archive->CloseKey(modelkey);
		}
	}
	else
	{
		hookupMesh(archive, model);
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

		if (vtype == 2)
		{
			visual = new CSubMesh();
		}
		else if (vtype == 1)
		{
			visual = new CMesh();
		}
		else if (vtype == 0 || vtype == -1)
		{
			visual = new CVisual();
		}

		if (visual != NULL)
		{
			visual->Read(archive);
		}

		return visual;
	}

	bool CVisual::Read(zar::CZAR& archive)
	{
		archive.Fetch("vparams", this, sizeof(tag_VIS_PARAMS));

		if (m_detail_buff == NULL)
		{
			zfree(m_detail_buff);
		}

		m_detail_buff = NULL;
		m_detail_cnt = 0;

		archive.Fetch("detail_cnt", &m_detail_cnt);
		
		if (m_detail_cnt != 0)
		{
			u32 detail_size = 0;
			archive.Fetch("detail_size", &detail_size);
			m_detail_buff = zmalloc(detail_size);
			archive.Fetch("detail_buff", m_detail_buff, detail_size);

			for (s32 i = 0; i < m_detail_cnt; i++)
			{

			}
		}

		return true;
	}

	bool CVisual::DrawLOD(zdb::CLOD_band* lod, f32 range, f32* distance)
	{
		bool inrange = false;

		if (lod->m_minRangeNearSq < range && range < lod->m_maxRangeFarSq)
		{
			if (lod->m_minFade)
			{
				inrange = true;
			}
			else if (range < lod->m_minRangeFarSq || (inrange = true, lod->m_maxRangeFarSq < range))
			{
				if (lod->m_minFade || lod->m_minRangeNearSq < range)
				{
					inrange = true;
					if (lod->m_maxFade)
					{
						if (lod->m_maxRangeNearSq <= range)
						{
							inrange = true;
							*distance = *distance * lod->m_minInvDeltaRangeSq * (range - lod->m_minRangeNearSq);
						}
					}
				}
			}
		}

		return inrange;
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
		{
			VuUpdate(1.0f);
		}
	}

	void CVisual::VuUpdate(f32 opacity)
	{
		glUseProgram(m_shader->m_ID);
		glBindVertexArray(m_vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
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
			{
				return true;
			}
		}

		return false;
	}

	void CVisData::NextVertex(u32 stride)
	{
		
	}
}