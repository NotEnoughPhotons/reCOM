#include "zvis.h"

namespace zdb
{
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

}
