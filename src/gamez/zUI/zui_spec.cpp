#include "zui.h"

#include "gamez/zUtil/util_stable.h"

CGameDlgDesign::CGameDlgDesign()
{
    m_useMpeg = false;
}

CDlgObjSpec* GetObjectSpec(_zrdr* reader, char* name)
{
    if (!name)
    {
        return NULL;
    }

    if (!strcmp("TEXT", name))
    {
        CTextSpec* text_spec = new CTextSpec();
        text_spec->m_uivar = NULL;

        if (reader)
        {
            CUIVariableSpec* uivar = NULL;
            auto uivar_tag = zrdr_findtag(reader, "UIVAR");

            if (uivar_tag)
            {
                uivar = new CUIVariableSpec(uivar_tag);
            }

            text_spec->m_uivar = uivar;
            text_spec->m_caption = "";
            text_spec->m_font = "";
            text_spec->m_scale = 1.0f;
            text_spec->m_hCentered = false;

            char* caption = zrdr_findstring(reader, "CAPTION");
            char* font = zrdr_findstring(reader, "FONT");

            if (caption)
            {
                text_spec->m_caption = caption;
            }
            else
            {
                text_spec->m_caption = "";
            }

            zrdr_findreal(reader, "SCALE", &text_spec->m_scale, 1);

            if (zrdr_findtag(reader, "HCENTERED"))
            {
                text_spec->m_hCentered = true;
            }

            if (zrdr_findtag(reader, "COLOR"))
            {
                zrdr_findPNT3D(reader, "COLOR", &text_spec->m_color);
            }
            else
            {
                text_spec->m_color.x = 128.0f;
                text_spec->m_color.y = 128.0f;
                text_spec->m_color.z = 128.0f;
            }

            if (font)
            {
                text_spec->m_hasFont = false;
                text_spec->m_font = "";
            }
            else
            {
                text_spec->m_hasFont = true;
                text_spec->m_font = font;
            }
        }

        return text_spec;
    }
    
    if (!strcmp("IMAGE", name))
    {
        
    }
    
    if (!strcmp("TELETYPE", name))
    {
        
    }
    
    if (!strcmp("WRAPPEDTEXT", name))
    {
        
    }
    
    if (!strcmp("TICKERTAPETEXT", name))
    {
        
    }
    
    if (!strcmp("PROGRESSBAR", name))
    {
        
    }
    
    if (!strcmp("COUNTER", name))
    {
        
    }
    
    if (!strcmp("CLOCK", name))
    {
        
    }
}

void LoadDlgObjects(std::vector<CDesignDlgObj*>& objects, _zrdr* reader, char* name, CButtonSpec*(*SpecGetter)(_zrdr*, char*))
{
    auto dlg_object = zrdr_findtag(reader, name);

    for (u32 i = 1; i < dlg_object->array->integer - 1; i++)
    {
        auto tag = &dlg_object->array[i];
        
        char* obj_name = zrdr_findstring(tag, "NAME");
        char* obj_type = zrdr_findstring(tag, "TYPE");
        s32 obj_xpos = 0;
        s32 obj_ypos = 0;

        zrdr_findint (tag, "XPOS", &obj_xpos);
        zrdr_findint (tag, "YPOS", &obj_ypos);

        auto spec_tag = zrdr_findtag(tag, "SPEC");

        if (spec_tag)
        {
            CDlgObjSpec* spec = SpecGetter(spec_tag, obj_name);

            if (spec)
            {
                objects.resize(objects.size() + 1);
            }
        }
    }
}

void CGameDlgDesign::LoadFromRdr(_zrdr* reader)
{
    if (auto background_tag = zrdr_findtag(reader, "BACKGROUND"))
    {
        char* type = zrdr_findstring(background_tag, "TYPE");
        char* filename = zrdr_findstring(background_tag, "FILENAME");
        
        if (type)
        {
            if (!SDL_strcasecmp(type, "IMAGE"))
            {
                m_useMpeg = false;
            }
            else if (!SDL_strcasecmp(type, "MPEG"))
            {
                m_useMpeg = true;
            }
        }

        if (filename)
        {
            m_filename = "";
        }
        else
        {
            m_filename = filename;
        }
    }

    if (auto anim_tag = zrdr_findtag(reader, "ANIMATIONS"))
    {
        for (u32 i = 1; i < anim_tag->array->integer; i++)
        {
            auto tag = &anim_tag->array[i];

            if (tag && tag->type == ZRDR_ARRAY)
            {
                
            }
        }
    }
}

CUIVariableSpec::CUIVariableSpec(_zrdr* reader)
{
    m_name = zrdr_findstring(reader, "NAME");
    m_array = NULL;
    m_stable = CSTable(0, 1024);

    _zrdr* var_value = zrdr_findtag(reader, "VALUE");

    if (var_value && var_value->type == ZRDR_ARRAY && var_value->array->integer == 2)
    {
        if (var_value->array->integer == 1)
        {
            var_value = NULL;
        }
        else
        {
            var_value = &var_value[1];
        }
    }

    if (var_value)
    {
        // m_array = new _zrdr(var_value, m_stable);
    }
}
