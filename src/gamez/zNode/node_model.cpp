#include "node_model.h"
#include "node_saveload.h"
#include "node_assetlib.h"

#include <algorithm>

namespace zdb
{
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

		if (library)
			library->AddModel(model);

		if (model->Read(sload))
			return model;

		if (library)
		{
			auto target = std::find(
				library->m_models.begin(),
				library->m_models.end(),
				model);

			library->m_models.erase(target);
		}

		return model;
	}

	bool CModel::Read(CSaveLoad& sload)
	{
		tag_NODE_PARAMS nparams;

		if (zar::CKey* modelkey = sload.m_zfile.GetOpenKey())
		{
			InitNodeParams(this);
			
			sload.m_zfile.Fetch("nparams", &nparams, sizeof(tag_NODE_PARAMS));
			sload.m_zfile.Fetch("regionmask", &m_region_mask, sizeof(u32));

			m_matrix = nparams.m_matrix;
			m_bbox.m_min = nparams.m_bbox.m_min;
			m_bbox.m_max = nparams.m_bbox.m_max;
			m_type = nparams.m_type;
			m_active = nparams.m_active;

			CNode::Read(sload);
			SetName(modelkey->GetName());
		}

		return true;
	}

	CModel* CModelVector::GetModel(const char* name)
	{
		CModel* model = NULL;

		if (name == NULL)
		{
			model = NULL;
		}
		else
		{
			for (auto it = begin(); it != end(); it++)
			{
				if (strcmp(name, (*it)->m_name) == 0)
				{
					model = *it;
					break;
				}
			}
		}

		return model;
	}
}