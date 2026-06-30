#include "zrdr.h"

#include "gamez/zUtil/zutil.h"
#include "gamez/zUtil/util_stable.h"
#include "gamez/zUtil/util_systemio.h"
#include "posix/strcasecmp.h"

std::list<zar::CZAR*> CRdrArchive::m_list;

zar::CZAR* CRdrArchive::AddArchive(const char* name, const char* path)
{
	char archiveName[256];
	CZAR* output = NULL;

	if (!path)
	{
		strcpy_s(archiveName, name);
	}
	else
	{
		sprintf_s(archiveName, 256, "%s/%s", path, name);
	}

	for (auto it = m_list.begin(); it != m_list.end(); it++)
	{
		CZAR* archive = *it;

		if (strcasecmp(archive->m_filename, archiveName) == 0)
		{
			output = archive;
			break;
		}
	}

	if (output == NULL)
	{
		output = new CZAR(archiveName, NULL);
		m_list.insert(m_list.end(), output);
	}

	return output;
}

bool CRdrArchive::RemoveArchive(const char* name, const char* path)
{
	char fullPath[256];
	sprintf_s(fullPath, 256, "%s/%s", path, name);

	auto it = m_list.begin();

	while (it != m_list.end())
	{
		CZAR* archive = *it;

		if (!strstr(archive->m_filename, fullPath))
		{
			++it;
			continue;
		}
		
		m_list.erase(it);
		archive->Close();

		if (archive)
		{
			delete archive;
		}

		break;
	}

	return false;
}

CRdrFile* CRdrArchive::FindRdr(const char* reader)
{
	CRdrFile* rdr = NULL;
	char keyName[1024];
	const char* subpath = strrchr(reader, '/');

	if (subpath == NULL)
	{
		if (keyName != NULL)
		{
			strcpy(keyName, reader);
		}
	}
	else if (subpath[1] != '\0')
	{
		if (keyName != NULL)
		{
			strcpy(keyName, subpath + 1);
		}
	}

	for (auto it = m_list.begin(); it != m_list.end(); ++it)
	{
		if (rdr)
		{
			return rdr;
		}

		bool open = false;

		zar::CZAR* archive = *it;

		if (!archive->m_pFileAlloc)
		{
			open = false;
		}
		else
		{
			open = archive->m_pFileAlloc->IsOpen();
		}

		if (!open)
		{
			archive->ReOpen(1, OpenFlags::READ);
		}

		rdr = NULL;
		zar::CKey* key = archive->OpenKey(keyName);
		if (key)
		{
			rdr = CRdrFile::Load(archive, key);
			archive->CloseKey(key);
		}

		if (!open)
		{
			archive->Close();
		}
	}

	return rdr;
}

void CRdrArchive::OpenAll()
{
	for (auto it = m_list.begin(); it != m_list.end(); it++)
	{
		CZAR* archive = *it;
		archive->ReOpen(1, OpenFlags::READ);
	}
}

void CRdrArchive::CloseAll()
{
	for (auto it = m_list.begin(); it != m_list.end(); it++)
	{
		zar::CZAR* archive = *it;
		archive->Close();
	}
}