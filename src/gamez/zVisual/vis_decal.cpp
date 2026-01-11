#include "zvis.h"

namespace zdb
{
    CDecalPool CDecal::m_tempPool;
    CMeshDecalPool CMeshDecal::m_tempPool;

    void CVisual::ApplyDecal(CDecal* decal)
    {
        CDecal* applied = NULL;

        for (auto i = m_decals.begin(); i != m_decals.end(); i++)
        {
            if (!(*i))
                return;

            if ((*i) == decal)
            {
                if (!applied)
                    m_decals[m_decal_idx] = *i;
                else
                    applied = *i;

                break;
            }
        }
    }

    // TODO: Implement this function
    void CDecalPool::RecycleTick()
    {

    }

    // TODO: Implement this function
    void CMeshDecalPool::RecycleTick()
    {

    }
}
