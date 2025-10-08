#include "zassetlib.h"

#include "gamez/zNode/znode.h"

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

}