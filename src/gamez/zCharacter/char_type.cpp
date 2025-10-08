#include <vector>
#include <algorithm>

#include "zchar.h"

#include "gamez/zReader/zrdr.h"

std::list<CCharacterType*> CCharacterType::m_char_list;

bool DeleteTemporaryCharType(CCharacterType* chartype)
{
    if (!chartype)
        return false;

    if (!chartype->m_istemporary)
        return false;

    delete chartype;
    return true;
}

CCharacterType::CCharacterType()
{
    m_name = NULL;
    m_ref_count = 0;
    m_model_file = NULL;
    m_model_file = NULL;
    m_parentID = 0;
    m_model = NULL;

    m_weapons.reserve(8);

    m_controller_id = 0;
}

CCharacterType::CCharacterType(const char* name, const CCharacterType* other)
{

}

bool CCharacterType::Open(const char* path)
{
    _zrdr* reader = zrdr_read(path);

    if (reader)
    {
        _zrdr* gear = zrdr_findtag(reader, "gear");

        if (gear)
        {
            s32 i = 1;
            
            do
            {
                _zrdr* cur = &gear->array[i];
                
                char* name = zrdr_findstring(cur, "name");
                char* model_name = zrdr_findstring(cur, "model");
                _zrdr* ofs = zrdr_findtag(cur, "ofs");
                i++;
            }
            while (i < gear->array->integer);
        }
    }

    return true;
}

CCharacterType* CCharacterType::Create(const char* name, const CCharacterType* other, bool destroyCopy)
{
    CCharacterType* ctype = GetByNameMod(name);

    if (!ctype)
    {
        ctype = new CCharacterType(name, other);
        m_char_list.push_back(ctype);
        return ctype;
    }

    return ctype;
}

CCharacterType* CCharacterType::GetByNameMod(const char* name)
{
    for (auto i = m_char_list.begin(); i != m_char_list.end(); i++)
    {
        CCharacterType* chartype = *i;

        if (strcmp(chartype->m_name, name))
            return chartype;
    }

    return NULL;
}

void CCharacterType::CleanupTemporary()
{
    CCharacterType::m_char_list.remove_if(DeleteTemporaryCharType);
}