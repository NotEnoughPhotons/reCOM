#ifndef NODE_ASSETLIB_H
#define NODE_ASSETLIB_H
#include <list>

#include "gamez/zNode/node_model.h"
#include "gamez/zTexture/ztex.h"
#include "gamez/zSystem/zsys.h"

enum class _RenderPhase;

namespace zdb
{
	class CModel;
	class CTexture;
	class CTexPalette;

	class CAssetLib;
}

extern void HookupLib(zdb::CAssetLib* lib);

namespace zdb
{
	class CAssetLib
	{
		friend class CAssetList;
	public:
		CAssetLib(const char* name);

		bool AddTexture(const char* name);
		bool AddTexture(CTexture* texture);
		CModel* AddModel(CModel* model);
		CTexPalette* AddPalette(CTexPalette* palette, bool checkDuplicate);

		bool IsNamed(const char* name) const;
		char* RootName() const;

		char* m_name;

		CModelVector m_models;
		CTexList m_textures;
		CTexPalList m_palettes;

		bool m_autoload;
		bool m_locked;
		bool m_gearlib;

		_RenderPhase m_renderphase;

		void* m_texture_buffer;
		void* m_model_buffer;

		s32 m_iRefCount;
	};

	class CAssetList : public std::list<CAssetLib*>
	{
	public:
		CTexHandle* GetTexHandle(const char* name);
		CAssetLib* FindLib(const char* name);
		CModel* GetModel(const char* name);

		CModel* m_cache_model;
	};

	class CAssetMgr
	{
	public:
		static CAssetLib* GetLoadedLibRef(const char* name);

		static CAssetList m_assets;
	};
}
#endif