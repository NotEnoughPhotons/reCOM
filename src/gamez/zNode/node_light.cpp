#include "node_light.h"
#include "node_saveload.h"

namespace zdb
{
    CLight::CLight()
    {
        m_type = (u32)TYPE::LIGHT;
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

void LightList_Search(zdb::CNode* node, zdb::CLightList& lightList)
{
    if (!node)
        return;

    if (node->m_type != (u32)zdb::CNode::TYPE::CELL)
        return;

    if (node->m_type == (u32)zdb::CNode::TYPE::LIGHT)
        lightList.insert(lightList.begin(), dynamic_cast<zdb::CLight*>(node));

    for (auto i = node->m_child.begin(); i != node->m_child.end(); ++i)
        LightList_Search(*i, lightList);
}