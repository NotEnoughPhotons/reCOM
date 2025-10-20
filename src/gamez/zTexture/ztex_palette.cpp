#include "ztex.h"
#include "gamez/zArchive/zar.h"
#include "gamez/zNode/node_saveload.h"

namespace zdb
{
    u32 CTexPalette::m_palId = 0;

    CTexPalette::CTexPalette(const char* name)
    {
        m_name = zstrdup(name);
        m_gsaddr = m_palId;
        m_slots = 0;
        m_buffer = 0;
        
        m_size = 0;
        m_format = 0;
        m_combo_pal = false;
        m_dynamic = false;

        m_AssetLib = NULL;
        m_palId++;
    }
    
    bool CTexPalette::Read(CSaveLoad& sload)
    {
        if (sload.m_zfile.Fetch("par", this, sizeof(PALETTE_PARAMS)))
        {
            m_buffer = zmalloc(m_size);
            sload.m_zfile.Fetch("buf", m_buffer, m_size);
        }
        else
        {
            return false;
        }

        m_id = m_gsaddr;
        return true;
    }
}
