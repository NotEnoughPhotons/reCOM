#include "zintersect.h"

#include "gamez/zArchive/zar.h"
#include "gamez/zNode/znode.h"
#include "gamez/zNode/node_saveload.h"

namespace zdb
{
    CDI::CDI()
    {
        m_nrm = CPnt4D::zero;
        m_region = 0;
        m_refcount = 0;
        m_ditype = 0;
        m_ptcount = 0;
        m_material = 0;
        m_cameratype = 0;
        m_appflags = 0;
        m_inside = false;
        m_shadow = 0;
        m_reserved = 0;

        m_pts = NULL;
    }
    
    CDI* CDI::Create(CSaveLoad& sload)
    {
        CDI* di = NULL;
        zar::CKey* dikey = sload.m_zfile.GetOpenKey();

        if (dikey)
        {
            di = new CDIPoly();

            // Failed to read?
            if (!di->Read(sload))
            {
                delete di;
                di = NULL;
            }
        }

        return di;
    }

    void CDI::Allocate(size_t size)
    {
        m_ptcount = size;
        m_pts = static_cast<CPnt4D*>(zmalloc(m_ptcount << 4));
    }
    
    bool CDI::Read(CSaveLoad& sload)
    {
        if (sload.m_zfile.GetOpenKey())
        {
            if (sload.m_zfile.Fetch("params", this, sizeof(DI_PARAMS)))
            {
                m_refcount = 0;

                if (!m_pts)
                {
                    return false;
                }
                
                return sload.m_zfile.Fetch("points", m_pts, m_ptcount);
            }
        }

        return false;
    }

    void CDI::Free()
    {
        if (m_pts)
        {
            zfree(m_pts);
        }

        m_pts = NULL;
        m_ditype = 0;
        m_ptcount = 0;
        m_material = 0;
        m_cameratype = 0;
        m_appflags = 0;
        m_inside = false;
        m_shadow = 0;
        m_reserved = 0;
    }

    DiIntersect::DiIntersect(s32 count)
    {
        m_BufCnt = count;
        m_Intersects = static_cast<IntersectStruct*>(zmalloc(count << 5));
        m_Node = NULL;
        m_Cnt = 0;

        m_Type = 0;
        m_IntersectCharacters = false;
        m_AltitudeCharacters = false;
        m_ProximityCharacters = false;
    }
    
    bool CDIVector::Exists(CDI* di)
    {
        const auto it = begin();

        while (it != end())
        {
            if (*it == di)
            {
                return true;
            }
        }

        return false;
    }

}
