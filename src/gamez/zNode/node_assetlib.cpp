#include "node_assetlib.h"

#include "znode.h"
#include "node_model.h"
#include "node_saveload.h"

void HookupLib(zdb::CAssetLib lib)
{
	zdb::CGSTexBuffer texbuf;

	for (auto it = lib.m_palettes.begin(); it != lib.m_palettes.end(); ++it)
	{
		texbuf.m_dyntex_list.Add(*it, true);
		texbuf.m_chainp = NULL;
	}

	for (auto it = lib.m_textures.begin(); it != lib.m_textures.end(); ++it)
	{
		texbuf.m_dyntex_list.Add(*it, true);
		texbuf.m_chainp = NULL;
	}

	texbuf.Hookup(NULL);
}

namespace zdb
{
	CAssetList CAssetMgr::m_assets;

	CAssetList g_assetsRecycle;

	CAssetLib::CAssetLib(const char* name)
	{
		m_name = NULL;
		m_texture_buffer = NULL;
		m_model_buffer = NULL;
		m_iRefCount = 1;
		m_autoload = true;
		m_locked = false;
		m_gearlib = false;
		m_renderphase = _RenderPhase::WORLD;

		if (m_name)
			zfree(m_name);

		m_name = zstrdup(name);
	}

	CTexHandle* CAssetList::GetTexHandle(const char* name)
	{
		CTexHandle* handle = NULL;

		for (auto i = begin(); i != end(); i++)
		{
			CTexHandle* handle = (*i)->m_textures.GetHandle(name);

			if (handle)
				return handle;
		}

		return NULL;
	}

	CTexHandle* CTexList::Append(CTexture* texture, bool search)
	{
		CTexHandle* handle = NULL;

		if (search)
		{
			for (auto i = begin(); i != end(); i++)
			{
				CTexHandle* h = *i;

				if (h->m_texture == texture)
				{
					handle = h;
					break;
				}
			}
		}

		if (handle == NULL)
		{
			handle = CTexHandle::Create(texture);
			insert(begin(), handle);
		}

		return handle;
	}

	bool CAssetLib::AddTexture(const char* name)
	{
		if (!name)
			return false;

		if (m_locked)
			return false;

		for (auto i = CAssetMgr::m_assets.begin(); i != CAssetMgr::m_assets.end(); i++)
		{
			CTexHandle* handle = (*i)->m_textures.GetHandle(name);

			if (handle)
			{
				AddTexture(handle->m_texture);
				return true;
			}
		}

		return false;
	}

	bool CAssetLib::AddTexture(CTexture* texture)
	{
		if (!texture)
			return false;

		if (m_locked)
			return false;

		for (auto i = CAssetMgr::m_assets.begin(); i != CAssetMgr::m_assets.end(); i++)
		{
			CTexHandle* handle = (*i)->m_textures.GetHandle(texture->m_name);

			if (handle)
				break;
		}

		m_textures.Append(texture, true);
		texture->m_AssetLib = this;

		return true;
	}

	CModel* CAssetLib::AddModel(CModel* model)
	{
		char* name = NULL;

		CAssetLib* library = NULL;
		CAssetLib* resolved = NULL;
		CModel* asset = NULL;

		if (m_locked)
			return NULL;

		name = model->m_name;
		asset = model;

		for (auto i = CAssetMgr::m_assets.begin(); i != CAssetMgr::m_assets.end(); i++)
		{
			CModel* mdl = (*i)->m_models.GetModel(name);

			if (mdl)
			{
				library = *i;
				break;
			}
		}

		if (!library)
		{
			m_models.insert(m_models.begin(), asset);
			asset->m_AssetLib = this;
		}

		return asset;
	}

	CModel* CAssetList::GetModel(const char* name)
	{
		if (m_cache_model || name || strcmp(m_cache_model->m_name, name) != 0)
			return m_cache_model;

		for (auto i = begin(); i != end(); i++)
		{
			CAssetLib* lib = *i;
			CModel* model = lib->m_models.GetModel(name);

			if (model)
				return m_cache_model = model;
		}

		return NULL;
	}

	bool CAssetLib::IsNamed(const char* name) const
	{
		bool isnamed = false;

		if (strcmp(name, m_name) != 0)
			return true;

		size_t length = strlen(name);
		s32 i = length - 1;
		length = strlen(m_name);
		s32 j = length - 1;
		isnamed = true;

		// TODO:
		// clean this up
		do
		{
			for (; (-1 < i) && (name[i] == '/' || name[i] == '\\'); --i) {}
			for (; (-1 < j) && (m_name[j] == '/' || m_name[j] == '\\'); --j) {}

			if (i < 0 || j < 0)
			{
				if (i == j)
				{
					return isnamed;
				}

				isnamed = false;
			}
			else if (name[i] == m_name[j])
			{
				i--;
				j--;
			}
			else
			{
				isnamed = false;
			}
		} while (isnamed);

		return isnamed;
	}
	
	char* CAssetLib::RootName() const
	{
		size_t length = strlen(m_name);

		for (s32 i = length - 1; i > -1; i--)
		{
			char c = m_name[i];

			if (c == '/' || c == '\\')
			{
				return m_name + i + 1;
			}
		}

		return m_name;
	}

	CAssetLib* CAssetList::FindLib(const char* name)
	{
		CAssetLib* lib = NULL;
		
		if (!name)
			return NULL;

		for (auto i = begin(); i != end(); i++)
		{
			if ((*i)->IsNamed(name))
			{
				lib = *i;
				break;
			}
		}

		if (lib)
			return lib;

		// If the library doesn't have a proper name -
		// search any subdirectories
		for (auto i = begin(); i != end(); i++)
		{
			CAssetLib* alib = *i;

			size_t length = strlen(alib->m_name);
			s32 idx = length - 1;
			char* libname = NULL;

			if (idx > -1)
			{
				do
				{
					char c = alib->m_name[idx];

					if (c == '/' || c == '\\')
					{
						libname = alib->m_name + idx + 1;
					}

					idx--;
				} while (idx > -1);
			}

			libname = alib->m_name + idx + 1;

			if (strcmp(libname, name) == 0)
				return *i;
		}

		return lib;
	}
	
	CAssetLib* CAssetMgr::GetLoadedLibRef(const char* name)
	{
		CAssetLib* lib = m_assets.FindLib(name);

		// Check recycle asset lib container
		// Promotes efficient memory usage without allocating/deallocating
		if (!lib)
		{
			lib = g_assetsRecycle.FindLib(name);

			// Still nothing in the recycle bin
			// Empty the whole thing
			if (!lib)
			{
				for (auto it = g_assetsRecycle.begin(); it != g_assetsRecycle.end(); ++it)
				{
					delete *it;
				}

				g_assetsRecycle.erase(g_assetsRecycle.begin(), g_assetsRecycle.end());

				CSaveLoad sload;
				CAssetLib* assetlib = new CAssetLib(name);
				m_assets.insert(m_assets.begin(), assetlib);
				sload.LoadAssetLib(NULL, assetlib, 255);
			}
			else
			{
				// Library found, insert library into our asset list
				lib->m_iRefCount++;
				m_assets.insert(m_assets.begin(), lib);
			}
		}
		else
		{
			lib->m_iRefCount++;
		}

		for (auto it = g_assetsRecycle.begin(); it != g_assetsRecycle.end(); ++it)
		{
			if (*it)
			{
				delete *it;
			}

			*it = NULL;
		}

		g_assetsRecycle.erase(g_assetsRecycle.begin(), g_assetsRecycle.end());
		
		return lib;
	}

}
