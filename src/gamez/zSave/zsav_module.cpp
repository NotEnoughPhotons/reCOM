#include "zsave.h"

CSaveModule::CSaveModule(const char* name, CSaveManager* manager)
{
	m_ModuleName = zstrdup(name);
	m_Priority = 0;
	m_Flags = 0;
	m_Manager = manager;

	if (m_Manager)
		m_Manager->RegisterModule(this);
}