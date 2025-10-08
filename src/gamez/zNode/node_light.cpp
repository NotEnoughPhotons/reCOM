#include "node_light.h"
#include "node_saveload.h"

namespace zdb
{
    CLight::CLight()
    {
        m_type = (u32)TYPE::NODE_TYPE_LIGHT;
        m_invMaxRange = 0.0f;
        m_maxRange = 0.0f;
        m_diffuse.x = 128.0f;
        m_diffuse.y = 128.0f;
        m_diffuse.z = 128.0f;
        m_opacity = 64.0f;
        m_appliedToNode = 0;
    }

    bool CLight::Read(CSaveLoad& sload)
    {
        return true;
    }
}