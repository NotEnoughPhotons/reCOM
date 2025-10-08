#include "zsave.h"

CSaveManager::CSaveManager(CIO* io)
{
	m_pFile = io;
	m_TempBufferSize = 0;
	m_TempBuffer = NULL;
}

void CSaveManager::RegisterModule(CSaveModule* module)
{

}