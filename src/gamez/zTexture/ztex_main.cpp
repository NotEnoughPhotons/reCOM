#include "ztex.h"

#include "gamez/zArchive/zar.h"
#include "gamez/zNode/node_saveload.h"
#include "gamez/zVideo/zvid.h"

namespace zdb
{
	u32 CTexture::m_baseAddress = 0;
	u32 CTexture::m_endAddress = 0;
	u32 CTexture::m_startAddress = 0;
	
	CTexture::CTexture(const char* name)
	{
		m_name = "UNNAMED_TEX";
		m_palette = NULL;

		m_texelBitSize = 0;
		m_selectQwc = 0;
		m_pal_offset = 0;
		m_transparent = false;
		m_palettized = false;
		m_is_mip_child = false;
		m_bumpmap = false;
		m_bilinear = false;
		m_transp_1bit = false;
		m_dynamic = false;
		m_context = false;

		m_width = 0;
		m_height = 0;
		m_gsaddr = 0;
		m_palID = 0;

		m_AssetLib = NULL;
		m_htex_count = 0;

		if (m_name && m_name != "UNNAMED_TEX")
		{
			zfree(m_name);
		}

		m_name = "UNNAMED_TEX";

		if (name)
		{
			m_name = zstrdup(name);
		}
	}

	CTexture::~CTexture()
	{
		
	}

	void CTexture::Init()
	{
		m_baseAddress = zVid.textureBaseAddr;
		m_endAddress = zVid.textureEndAddr;
		m_startAddress = zVid.textureBaseAddr + 0x200;
	}

	bool CTexture::Read(zdb::CSaveLoad& sload)
	{
		_word128 texGifPtrs;
		bool success = false;

		if (zar::CKey* dataKey = sload.m_zfile.FindKey("texdat"))
		{
			sload.m_zfile.FetchLIP(dataKey, reinterpret_cast<void**>(&texGifPtrs));

			memcpy(this, &texGifPtrs, sizeof(TEXTURE_PARAMS));
			zmalloc(96);

			_word128* texGifPtr = &texGifPtrs;
			void* bufferPtr = static_cast<void*>(&texGifPtr[1].u32[0]);

			m_buffer = static_cast<void*>(bufferPtr);
		}

		switch (m_texelBitSize)
		{
		case 4:
			m_format = 20;
			break;
		case 8:
			m_format = 19;
			break;
		case 16:
			m_palettized = true;
			break;
		case 24:
			m_format = 1;
			goto nopal;
		case 32:
			m_format = 0;
			goto nopal;
		}

		if (!m_transp_1bit)
			m_format = 19;
		else
			m_format = 2;
nopal:
		m_palID = 0;
		return false;
	}
	
	u16 CTexture::Release_HTEX()
	{
		u16 count = m_htex_count;
		m_htex_count = count - 1;
		return count - 1;
	}

	CTexHandle* CTexHandle::Create(CTexture* texture)
	{
		CTexHandle* handle = new CTexHandle();

		handle->m_name = NULL;
		handle->m_count = 0;
		handle->m_texture = NULL;
		handle->m_libref = false;

		if (handle->m_texture != NULL)
		{
			handle->m_texture->Release_HTEX();
		}

		handle->m_texture = texture;

		if (texture != NULL)
		{
			char* name = texture->m_name;

			if (handle->m_name != NULL)
			{
				zfree(handle->m_name);
			}

			handle->m_name = NULL;

			if (name == NULL)
			{
				handle->m_name = NULL;
			}
			else
			{
				handle->m_name = strdup(name);
			}

			texture->m_htex_count++;
		}

		handle->m_count++;
		return handle;
	}

	void CDynTexList::Add(CTexPalette* palette, bool check)
	{

	}

	void CDynTexList::Add(CTexHandle* handle, bool check)
	{

	}

	void CDynTexList::Add(CDynTexList* list)
	{

	}

	CGSTexBuffer::CGSTexBuffer()
	{
		m_checkForOverflow = true;
		m_startAddress = 0;
		m_nextAddress = 0;
		m_endAddress = 0;
		m_assetLib = NULL;
		m_chainp = NULL;
		m_name = NULL;
	}

	void CGSTexBuffer::Hookup(CAssetLib* lib)
	{
		
	}

	CTexHandle* CTexList::GetHandle(const char* name)
	{
		CTexHandle* handle = NULL;
		
		auto it = begin();

		while (it != end())
		{
			CTexHandle* cur_handle = *it;

			if (!cur_handle->m_texture || !cur_handle->m_name)
			{
				++it;
				continue;
			}

			if (SDL_strcasecmp(cur_handle->m_name, name) == 0)
			{
				handle = cur_handle;
				break;
			}
			
			++it;
		}

		return handle;
	}

}
