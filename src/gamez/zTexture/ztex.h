#pragma once
#include <vector>
#include <SDL3/SDL.h>
#include <SDL3/SDL_pixels.h>

#include "gamez/zSystem/zsys.h"

namespace zar
{
	class CZAR;
}

namespace zdb
{
	class CAssetLib;
	class CSaveLoad;
}

namespace zdb
{
	struct TEXTURE_PARAMS
	{
		u16 m_width;
		u16 m_height;
		u32 m_size;
		u32 m_gsaddr;
		u32 m_texelBitSize : 8;
		u32 m_selectQwc : 8;
		u32 m_pal_offset : 8;
		u32 m_transparent : 1;
		u32 m_palettized : 1;
		u32 m_is_mip_child : 1;
		u32 m_bumpmap : 1;
		u32 m_bilinear : 1;
		u32 m_transp_1bit : 1;
		u32 m_dynamic : 1;
		u32 m_context : 1;
	};

	struct PALETTE_PARAMS
	{
		u32 m_gsaddr;
		u32 m_size : 16;
		u32 m_format : 8;
		u32 m_combo_pal : 1;
		u32 m_dynamic : 1;
		u32 m_unused : 6;
	};

	class CLoadImage
	{
		
	};
	
	class CTexPalette : public PALETTE_PARAMS
	{
	public:
		bool Read(CSaveLoad& sload);
		bool Read(zar::CZAR& archive);
		
		CAssetLib* m_AssetLib;

		char* m_name;
		s32 m_id;
		void* m_buffer;
		u32 m_slots;
	};

	class CTexture : public TEXTURE_PARAMS
 	{
		friend class CTexHandle;
	public:
		CTexture() {}
		CTexture(const char* name);
		~CTexture();
		
		static void Init();

		u16 Release_HTEX();

		bool Read(CSaveLoad& saveload);
		bool Read(zar::CZAR& archive);

		static u32 m_baseAddress;
		static u32 m_endAddress;
		static u32 m_startAddress;
		
		char* m_name;
		CAssetLib* m_AssetLib;
		void* m_buffer;
		CTexPalette* m_palette;
		_word128* m_gifSelect;
		_word128* m_vuSelect;
		SDL_PixelFormat m_format;
		u16 m_palID;
		u16 m_htex_count;
		u16 m_gsWords;
		_word128 m_dmaRef;
		_word128 m_dmaRefVu;
		std::vector<CTexture*> m_reverse_miplist;
		CLoadImage* m_loadimage;
	};

	class CTexList : public std::vector<CTexHandle*> 
	{
	public:
		CTexList() {}

		CTexHandle* Append(CTexture* texture, bool search);
		CTexHandle* GetHandle(const char* name);
	};

	class CTexPalList : public std::vector<CTexPalette*>
	{
	public:
		CTexPalList() {}
	};

	class CDynTexList
	{
	public:
		void Add(CTexPalette* palette, bool check);
		void Add(CTexHandle* handle, bool check);
		void Add(CDynTexList* list);
	private:
		CTexList m_tex_list;
		CTexPalList m_pal_list;
	};

	class CGSTexBuffer
	{
	public:
		CGSTexBuffer();

		void Hookup(CAssetLib* lib);
		
		char* m_name;

		bool m_checkForOverflow;

		u32 m_startAddress;
		u32 m_nextAddress;
		u32 m_endAddress;

		zdb::CAssetLib* m_assetLib;

		s64* m_chainp;

		std::vector<void*> m_pktbuf;

		CDynTexList m_dyntex_list;
	};

	struct tag_TexLoadCmds
	{
		CGSTexBuffer* pTexAsset;
		CGSTexBuffer* pTexAssetUsed;
		u32* pWaitTime;
	};

	class CTexManager : public std::vector<CDynTexList*>
	{
	public:
		static CTexManager* m_texmanager;

		void doAddBuffer(const char* name, f32 param_2, f32 param_3);

		u32 m_renderMapNum;
		u32 m_renderMapNumBytes;
		u32 m_shadowMapSize;
		u32 m_reflectionMapSize;
	};

	class CTexHandle
	{
	public:
		static CTexHandle* Create(CTexture* texture);
		
		char* m_name;
		s32 m_count;
		u32 m_gsAddr;
		CTexture* m_texture;
		bool m_libref;
	};

	class CTextureRelocMgr
	{
	public:
		virtual void AddToBatch(zdb::CTexHandle* handle) const {}
		virtual void ClearAll() {}
		virtual void EndBatchReloc() {}
		virtual void PreGetSprPacket() {}
		virtual void StartBatchReloc() {}
	};

	class CBatchRelocator : public CTextureRelocMgr
	{
	public:
		void AddToBatch(zdb::CTexHandle* handle) const;
		void ClearAll();
		void EndBatchReloc();
		void StartBatchReloc();
		void TakeDrawPacket(void* spr, int qwc);
	};
	
	class CTextureFX
	{
	public:
		static void Init();
	};

	class CRipple
	{
	public:
		static void Init();
	};
}