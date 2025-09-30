#include "znode.h"
#include "gamez/zAssetLib/zassetlib.h"

#include "gamez/zSave/zsave.h"

namespace zdb
{
	CNode* CNode::CreateInstance(CSaveLoad& sload)
	{
		tag_NODE_PARAMS nparams;
		CNode* node = NULL;
		char* modelname = NULL;
		u32 regionmask = 0;

		if (!CWorld::m_world)
			return NULL;

		if (!&sload.m_zfile)
			return NULL;

		InitNodeParams(&nparams, NULL);

		nparams.m_matrix = CMatrix::identity;

		sload.m_zfile.Fetch("model_name", &modelname);
		sload.m_zfile.Fetch("nparams", &nparams, sizeof(tag_NODE_PARAMS));
		sload.m_zfile.Fetch("regionmask", &regionmask);

		if (!modelname)
			return NULL;

		CModel* model = CWorld::GetModel(modelname);

		if (!model)
			return NULL;

		if (!modelname)
			zfree(modelname);

		modelname = NULL;

		node = CreateInstance(model, NULL, NULL);
		node->m_matrix = nparams.m_matrix;
		node->m_bbox.m_min = nparams.m_bbox.m_min;
		node->m_bbox.m_max = nparams.m_bbox.m_max;
		node->m_type = nparams.m_type;
		node->ReadDataBegin(sload);

		return node;
	}
	
	CNode* CNode::CreateInstance(const char* name, const CPnt3D* position, const CPnt3D* rotation)
	{
		return NULL;
	}

	CNode* CNode::CreateInstance(CModel* model, const CPnt3D* position, const CPnt3D* rotation)
	{
		return NULL;
	}

	CNode* CNode::Read(CSaveLoad& sload, CNode* parent)
	{
		tag_NODE_PARAMS nparams;
		CNode* node = NULL;
		u32 nodetype = 0;
		u32 regionmask = 0;

		bool isChild = false;

		if (sload.m_zfile.Fetch("NodeType", &nodetype))
		{
			sload.m_version = 1;

			nparams.m_matrix = CMatrix::identity;
			InitNodeParams(&nparams, NULL);

			sload.m_zfile.Fetch("nparams", &nparams, sizeof(tag_NODE_PARAMS));
			sload.m_zfile.Fetch("regionmask", &regionmask);

			switch ((CNode::TYPE)nparams.m_type)
			{
			case TYPE::NODE_TYPE_CHILD:
				isChild = true;
				break;
			case TYPE::NODE_TYPE_GENERIC:
			case TYPE::NODE_TYPE_UNK5:
				node = new CNode();
				break;
			case TYPE::NODE_TYPE_LIGHT:
				node = new CLight();
				break;
			case TYPE::NODE_TYPE_LENSFLARE:
				node = new CLensFlare();
				break;
			}

			if (isChild)
			{
				CNode* instance = CreateInstance(sload);

				if (instance)
				{
					if (!node)
					{
						CWorld::m_world->AddChild(instance);
					}
					else
					{
						parent->AddChild(instance);
					}
				}
			}
			else if (node)
			{
				nparams.m_matrix = node->m_matrix;
				nparams.m_bbox.m_min = node->m_bbox.m_min;
				nparams.m_bbox.m_max = node->m_bbox.m_max;
				nparams.m_type = node->m_type;
				nparams.m_active = node->m_active;
				nparams.m_dynamic_motion = node->m_dynamic_motion;
				nparams.m_dynamic_light = node->m_dynamic_light;
				nparams.m_landmark = node->m_landmark;
				nparams.m_light = node->m_light;
				nparams.m_prelight = node->m_prelight;
				nparams.m_fog = node->m_fog;
				nparams.m_transparent = node->m_transparent;
				nparams.m_facade = node->m_facade;
				nparams.m_reflective = node->m_reflective;
				nparams.m_bumpmap = node->m_bumpmap;
				nparams.m_hasDI = node->m_hasDI;
				nparams.m_region_shift = node->m_region_shift;
				nparams.m_has_visuals_prior_to_export = node->m_has_visuals_prior_to_export;
				nparams.m_shadow = node->m_shadow;
				nparams.m_worldchild = node->m_worldchild;
				nparams.m_char_common = node->m_char_common;
				nparams.m_NOTUSED = node->m_NOTUSED;
				nparams.m_hasVisuals = node->m_hasVisuals;
				nparams.m_hasMesh = node->m_hasMesh;
				nparams.m_scrolling_texture = node->m_scrolling_texture;
				nparams.m_light_dynamic = node->m_light_dynamic;
				nparams.m_light_static = node->m_light_static;
				nparams.m_clutter = node->m_clutter;
				nparams.m_mtx_is_identity = node->m_mtx_is_identity;
				nparams.m_use_parent_bbox = node->m_use_parent_bbox;
				nparams.m_apply_clip = node->m_apply_clip;
				node->m_region_mask = regionmask;

				if (!node->Read(sload))
				{
					if (node)
						delete node;
					
					node = NULL;
				}
				else if (parent)
					parent->AddChild(node);
			}
		}
		else
		{
			sload.m_version = 0;

			zar::CKey* key = sload.m_zfile.GetOpenKey();
			sload.m_zfile.OpenKey(key);
			sload.m_zfile.Fetch("NodeType", &nodetype);

			node = new CNode();

			if (node->Read(sload) && parent)
				parent->AddChild(node);

			sload.m_zfile.CloseKey(key);
		}

		return node;
	}

	bool CNode::Read(CSaveLoad& sload)
	{
		if (sload.m_version == 1)
		{
			ReadDataBegin(sload);

			zar::CKey* childrenkey = sload.m_zfile.OpenKey("children");

			if (childrenkey)
			{
				ReserveChildren(childrenkey->GetSize());

				auto it = childrenkey->begin();

				while (it != childrenkey->end())
				{
					zar::CKey* key = sload.m_zfile.OpenKey(*it);
					Read(sload, this);
					sload.m_zfile.CloseKey(key);

					it++;
				}

				sload.m_zfile.CloseKey(childrenkey);
			}
		}
		else
		{
			zar::CKey* openkey = sload.m_zfile.GetOpenKey();
			SetName(openkey->GetName());
			sload.m_zfile.Fetch("matrix", &m_matrix, sizeof(CMatrix));
			sload.m_zfile.Fetch("bbox", &m_bbox, sizeof(CBBox));
			zar::CKey* visualskey = sload.m_zfile.OpenKey("visuals");

			// If the node has visuals
			if (visualskey)
			{
				ReserveVisuals(visualskey->GetSize());
				auto it = visualskey->begin();

				while (it != visualskey->end())
				{
					zar::CKey* key = sload.m_zfile.OpenKey(*it);
				
					if (key)
					{
						CVisual* vis = CVisual::Create(sload.m_zfile);
						AddVisual(vis);
						sload.m_zfile.CloseKey(key);
					}

					it++;
				}

				sload.m_zfile.CloseKey(visualskey);
			}

			zar::CKey* dikey = sload.m_zfile.OpenKey("di");

			// If the node has collision data
			if (dikey)
			{
				ReserveDI(dikey->GetSize());

				auto it = dikey->begin();

				while (it != dikey->end())
				{
					zar::CKey* key = sload.m_zfile.OpenKey(*it);

					if (key)
					{
						CDI* di = CDI::Create(sload);
						AddDI(di);
						sload.m_zfile.CloseKey(key);
					}

					it++;
				}

				sload.m_zfile.CloseKey(dikey);
			}
		}

		zar::CKey* childrenkey = sload.m_zfile.OpenKey("children");

		// If the node has children
		if (childrenkey)
		{
			ReserveChildren(childrenkey->GetSize());

			auto it = childrenkey->begin();

			while (it != childrenkey->end())
			{
				zar::CKey* key = sload.m_zfile.OpenKey(*it);
				Read(sload, this);
				sload.m_zfile.CloseKey(key);

				it++;
			}

			sload.m_zfile.CloseKey(childrenkey);
		}
		
		return true;
	}

	CModel::CModel(const char* name)
	{
		m_AssetLib = NULL;
		m_type = 7;
		m_bbox_valid = false;
		SetName(name);
		m_variant = 0;
		m_bForceExport = false;
	}
	
	CModel* CModel::Create(CSaveLoad& sload, CAssetLib* library)
	{
		CModel* model = new CModel(NULL);

		if (library && model)
		{
			library->AddModel(model);
		}

		if (!model->Read(sload))
		{
			// TODO: Destroy the model after adding to the list. Node data is invalid.

			model = NULL;
		}

		return model;
	}
	
	bool CModel::Read(CSaveLoad& sload)
	{
		zar::CKey* modelkey = sload.m_zfile.GetOpenKey();
		if (modelkey != NULL)
		{
			CNode::Read(sload);
			SetName(modelkey->GetName());
		}

		return true;
	}
}