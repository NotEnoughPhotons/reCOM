﻿#include "ztwod.h"
#include "gamez/zVideo/zvid.h"

#include <SDL3/SDL_ttf.h>

C2DString::C2DString()
{
    m_string = (char*)zmalloc(17);
    m_stringlen_max = 16;
    m_numChars = 0;
    m_Ustring = (char*)zmalloc(17);
    *m_Ustring = '\0';
    m_Ustringlen_max = 16;
    m_numUChars = 0;
    m_font = NULL;
    m_scale = 1.0f;
    m_revnum = 0;
    m_color.x = 255.0f;
    m_color.y = 255.0f;
    m_color.z = 255.0f;
    m_color.w = 128.0f;
    m_firstCharPos = 0;
    m_elapsedTime = 0.0f;
}

void C2DString::AddCharacter(char character)
{
    m_numChars += 1;

    m_string = (char*)zrealloc(m_string, m_numChars + 1);

    if (m_numChars < 2)
    {
        *m_string = character;
        m_string[1] = '\0';
    }
    else
    {
        m_string[m_numChars - 1] = character;
        m_string[m_numChars] = '\0';
    }

    if (character != '\n')
    {
        s32 length = static_cast<s32>(m_length);
        C2DFontEntry* entry = m_font->GetEntry(character);

        if (entry)
        {
            if (entry->m_displaywidth == -1)
            {
                length = static_cast<s32>(length + m_scale * entry->m_width * m_font->m_scale_factor);
            }
            else
            {
                length += entry->m_displaywidth;
            }

            m_length += static_cast<f32>(length + entry->m_xspacing + entry->m_rKerning >> 4);
        }
    }
}

void C2DString::Draw(zdb::CCamera* camera)
{
    SDL_SetRenderScale(theWindow->GetRenderer(), m_scale, m_scale);
    SDL_GetTextureSize(m_strTexture, &m_rect.w, &m_rect.h);
    SDL_SetRenderDrawColor(theWindow->GetRenderer(), m_color.x, m_color.y, m_color.z, 255);
    SDL_RenderTexture(theWindow->GetRenderer(), m_strTexture, NULL, &m_rect);
}

void C2DString::SetString(const char* string)
{
    m_string = zstrdup(string);
}

void C2DString::Load(const char* message, C2DFont* font, s32 x, s32 y)
{
    m_pos.x = x;
    m_pos.y = y;
    SetString(message);
    m_font = font;

    if (m_color.w >= 128.0f)
    {
        SetTrans(m_color.w);
    }
}

void C2DString::Load(const char* message, C2DTTFFont* font, s32 x, s32 y, s32 width, s32 height)
{
    m_pos.x = x;
    m_pos.y = y;
    SetString(message);
    m_ttf_font = font;

    if (m_color.w >= 128.0f)
    {
        SetTrans(m_color.w);
    }

    m_charwidth = width;
    m_charheight = height;
}

void C2DString::Load(const char* message, C2DTTFFont* font, s32 x, s32 y)
{
    m_pos.x = x;
    m_pos.y = y;
    SetString(message);
    m_ttf_font = font;

    if (m_color.w >= 128.0f)
    {
        SetTrans(m_color.w);
    }

    SDL_Surface* text = NULL;
    
    SDL_Color color;
    color.r = static_cast<u8>(m_color.x);
    color.g = static_cast<u8>(m_color.y);
    color.b = static_cast<u8>(m_color.z);
    color.a = static_cast<u8>(m_color.w);
    text = TTF_RenderText_Blended(font->m_ttf_font, message, 0, color);

    m_rect.x = x;
    m_rect.y = y;
    
    if (text)
    {
        m_strTexture = SDL_CreateTextureFromSurface(theWindow->GetRenderer(), text);
        SDL_DestroySurface(text);
    }
    
    if (!m_strTexture)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
    }
}

void C2DString::Load(f32 scale, const char* message, C2DTTFFont* font, s32 x, s32 y)
{
    m_scale = scale;
    Load(message, font, x, y);
}

void C2DString::Load(const char* message, C2DFont* font, s32 x, s32 y, s32 width, s32 height)
{
    m_pos.x = x;
    m_pos.y = y;
    SetString(message);
    m_font = font;

    if (m_color.w >= 128.0f)
    {
        SetTrans(m_color.w);
    }

    m_charwidth = width;
    m_charheight = height;
}

void C2DString::Load(f32 scale, const char* message, C2DFont* font, s32 x, s32 y)
{
    m_scale = scale;
    Load(message, font, x, y);
}

