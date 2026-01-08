#include "zbody.h"

#include "gamez/zNode/znode.h"
#include "gamez/zMath/zmath.h"

CZBodyPart* CBody::FindPart(const char* name)
{
	for (auto it = m_body_parts.begin(); it != m_body_parts.end(); it++)
	{
		CZBodyPart* part = *it;

		if (strcmp(part->m_node->m_name, name) == 0)
		{
			return part;
		}
	}

	return NULL;
}

CZBodyPart::CZBodyPart(zdb::CNode* node)
{
	m_id = -1;
	m_blendme = 1;
	m_node = node;
	m_parent = NULL;
	m_pos = CPnt3D::zero;
	m_rot = CQuat::identity;
}

void CZBodyPart::Orient()
{
	if (this != NULL)
	{
		m_node->SetPosition(m_pos.x, m_pos.y, m_pos.z);
	}

	if (&m_rot != NULL)
	{
		m_node->SetRotation(&m_rot);
	}
}