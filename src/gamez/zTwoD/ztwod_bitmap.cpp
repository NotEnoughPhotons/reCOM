#include "ztwod.h"
#include "gamez/zVideo/zvid.h"

C2DBitmap::C2DBitmap()
{
	m_pTexHandle = NULL;
	m_iWidth = 0;
	m_iHeight = 0;
	m_x = 0;
	m_y = 0;

	m_NewUV[0][0] = 0.01f;
	m_NewUV[0][1] = -0.01f;
	m_NewUV[0][2] = 0.0f;
	m_NewUV[0][3] = 0.0f;
	m_NewUV[1][0] = 0.99f;
	m_NewUV[1][1] = -0.99f;
	m_NewUV[1][2] = 1.0f;
	m_NewUV[1][3] = 0.0f;

	m_RGB[0][0] = 255.0f;
	m_RGB[0][1] = 255.0f;
	m_RGB[0][2] = 255.0f;
	m_RGB[0][3] = 100.0f;
	m_RGB[1][0] = 255.0f;
	m_RGB[1][1] = 255.0f;
	m_RGB[1][2] = 255.0f;
	m_RGB[1][3] = 100.0f;
	m_RGB[0][0] = 128.0f;
	m_RGB[0][1] = 128.0f;
	m_RGB[0][2] = 128.0f;
	m_RGB[1][0] = 128.0f;
	m_RGB[1][1] = 128.0f;
	m_RGB[1][2] = 128.0f;

	SetTrans(128.0f);
}

void C2DBitmap::Load(f32 x, f32 y, f32 width, f32 height, zdb::CTexHandle* handle)
{
	m_pTexHandle = handle;
	
	m_x = static_cast<s32>(x);
	m_y = static_cast<s32>(y);
	m_iWidth = static_cast<s32>(width) - m_x;
	m_iHeight = static_cast<s32>(height) - m_y;
	
	if (m_hasTexture)
	{
		zdb::CTexture* texture = NULL;

		if (handle != NULL)
		{
			texture = handle->m_texture;

			s32 pitch = 0;

			if (texture->m_format == SDL_PIXELFORMAT_RGBA32 || texture->m_format == SDL_PIXELFORMAT_RGBA8888)
			{
				pitch = texture->m_width * sizeof(u8) * 4;
			}
			else if (texture->m_format == SDL_PIXELFORMAT_RGB24)
			{
				pitch = texture->m_width * sizeof(u8) * 3;
			}
			else if (texture->m_format == SDL_PIXELFORMAT_RGBA5551 || texture->m_format == SDL_PIXELFORMAT_RGBA4444 || texture->m_format == SDL_PIXELFORMAT_BGRA5551)
			{
				pitch = texture->m_width * sizeof(u8) * 2;
			}
			else if (texture->m_format == SDL_PIXELFORMAT_INDEX8)
			{
				pitch = texture->m_width * sizeof(u8);
			}

			SDL_Palette* palette = SDL_CreatePalette(8);
			SDL_Surface* surface = SDL_CreateSurfaceFrom(texture->m_width, texture->m_height, SDL_PIXELFORMAT_INDEX8, texture->m_buffer, texture->m_width * sizeof(u8));
			SDL_SetSurfacePalette(surface, palette);
			
			if (!surface)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
				return;
			}
			
			/*m_pSDLTexture = SDL_CreateTextureFromSurface(theWindow->GetRenderer(), surface);

			if (!m_pSDLTexture)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
				return;
			}*/
			
			if (texture->m_bilinear)
			{
				/*SDL_SetTextureScaleMode(m_pSDLTexture, SDL_SCALEMODE_LINEAR);*/
			}
			else
			{
				/*SDL_SetTextureScaleMode(m_pSDLTexture, SDL_SCALEMODE_NEAREST);*/
			}

			m_hasTexture = true;
			
			SDL_DestroySurface(surface);
		}

		if (texture == NULL && first)
		{
			first = false;
		}
	}
}

void C2DBitmap::Load(f32 x, f32 y, zdb::CTexHandle* handle)
{
	zdb::CTexture* texture = NULL;

	m_pTexHandle = handle;

	if (!m_hasTexture)
	{
		if (handle != NULL)
		{
			texture = handle->m_texture;
		}

		if (texture == NULL && !first)
		{
			return;
		}

		first = false;
		return;
	}

	if (m_pTexHandle == NULL)
	{
		texture = NULL;
	}
	else
	{
		texture = m_pTexHandle->m_texture;
	}

	if (texture != NULL)
	{
		m_x = static_cast<s32>(x);
		m_y = static_cast<s32>(y);
		m_iWidth = m_pTexHandle->m_texture->m_width;
		m_iWidth = m_pTexHandle->m_texture->m_height;
	}
}

void C2DBitmap::Draw(const CMatrix& transform, zdb::CTextureRelocMgr* reloc)
{
	zdb::CTexture* texture = NULL;

	if (!IsOn())
	{
		return;
	}

	if (!m_hasTexture)
	{
		if (m_pTexHandle == NULL)
		{
			texture = NULL;
		}
		else
		{
			texture = m_pTexHandle->m_texture;
		}

		if (texture == NULL)
		{
			if (first)
			{
				first = false;
			}

			C2D::Draw(m_p2dcamera);
			return;
		}
	}

	MakePacket(m_p2dcamera, reloc, transform);
	C2D::Draw(m_p2dcamera);
}

void C2DBitmap::Draw(zdb::CCamera* camera)
{
	if (!IsOn())
	{
		return;
	}

	if (!m_hasTexture)
	{
		zdb::CTexture* texture = NULL;
		
		if (!m_pTexHandle)
		{
			texture = NULL;
		}
		else
		{
			texture = m_pTexHandle->m_texture;
		}

		if (!texture)
		{
			if (first)
			{
				first = false;
			}

			/*SDL_SetRenderDrawColor(theWindow->GetRenderer(), m_RGB[0][0], m_RGB[0][1], m_RGB[0][2], 255);
			SDL_RenderFillRect(theWindow->GetRenderer(), &m_rect);*/
		}
	}
	else
	{
		/*SDL_RenderTexture(theWindow->GetRenderer(), m_pSDLTexture, NULL, &m_rect);*/
	}
}

void C2DBitmap::TickFade()
{
	float alpha = 0.0f;
	float limit = 0.0f;

	if (m_fade_enable)
	{
		float alpha = m_RGB[0][3] + m_fade_dx;

		if (m_fade_dx < 0.0f)
		{
			float limit = m_fade_limit;

			if ((m_fade_limit <= alpha) && (limit = alpha, alpha < 0.0f))
			{
				limit = 0.0f;
			}
		}
		else
		{
			float limit = m_fade_limit;

			if ((alpha <= m_fade_limit) && (limit = 128.0f, alpha <= 128.0f))
			{
				limit = alpha;
			}
		}

		SetTrans(limit);
	}
}

void C2DBitmap::MakePacket(zdb::CCamera* camera, zdb::CTextureRelocMgr* reloc, const CMatrix& transform)
{
	// TODO:
	// Add more to this function

	reloc->AddToBatch(m_pTexHandle);
}

void C2DBitmap::Reset()
{
	m_x = 0;
	m_y = 0;
	m_pTexHandle = NULL;
	m_iWidth = 0;
	m_iHeight = 0;
}

bool C2DBitmap::GetTrans() const
{
	return m_isTrans;
}

void C2DBitmap::SetTrans(float transparency)
{
	m_RGB[0][3] = transparency;
	m_RGB[1][3] = transparency;
	m_isTrans = true;
}

void C2DBitmap::SetPos(s32 x, s32 y, s32 width, s32 height)
{
	m_x = x;
	m_y = y;
	m_iWidth = width - x;
	m_iHeight = height - y;
}

void C2DBitmap::SetPos(s32 x, s32 y)
{
	zdb::CTexture* texture = NULL;

	if (m_pTexHandle == NULL)
	{
		texture = NULL;
	}
	else
	{
		texture = m_pTexHandle->m_texture;
	}

	if (texture != NULL)
	{
		m_x = x;
		m_y = y;
		m_iWidth = m_pTexHandle->m_texture->m_width;
		m_iWidth = m_pTexHandle->m_texture->m_height;
	}
}

void C2DBitmap::SetUV(f32 u0, f32 v0, f32 u1, f32 v1)
{
	m_NewUV[0][0] = u0;
	m_NewUV[0][1] = -v0;
	m_NewUV[1][0] = u1;
	m_NewUV[1][1] = -v1;
}

void C2DBitmap::SetColor(f32 red, f32 green, f32 blue)
{
	m_RGB[0][0] = red;
	m_RGB[0][1] = green;
	m_RGB[0][2] = blue;
	m_RGB[1][2] = red;
	m_RGB[1][2] = green;
	m_RGB[2][2] = blue;
}