#include "node_model.h"
#include "node_saveload.h"
#include "node_assetlib.h"

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