#include "zseal.h"

#include "../zCharacter/zchar.h"

CCharacterGear* CZSealBody::FindGear(const char* name)
{
	for (auto i = CCharacterType::m_gear_list.begin(); i != CCharacterType::m_gear_list.end(); ++i)
	{
		CCharacterGear* gear = *i;
		if (!strcmp(name, gear->m_name))
			return gear;
	}

	return NULL;
}