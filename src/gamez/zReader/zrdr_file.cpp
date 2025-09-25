#include "zrdr.h"
#include "Apps/FTS/gamever.h"

#include "gamez/zSystem/zsys.h"
#include "gamez/zUtil/util_stack.h"

char* buffer = NULL;

s32 CRdrArchive::version = 1;

CRdrFile::CRdrFile() : _zrdr()
{
	m_buffer = NULL;
	m_size = 0;
}

const char* zrdr_findfile(const char* name, const char* dir)
{
	if (dir)
	{
		sprintf_s(buffer, 64, "%s/%s", name, dir);
		name = buffer;
	}

	return name;
}

int zrdr_free(CRdrFile* file)
{
	if (file != NULL)
	{
		if (file->m_buffer == NULL)
		{
			zrdr_freearray(file);
		}
		else
		{
			zfree(file->m_buffer);
			file->m_buffer = NULL;
			file->m_size = 0;
		}

		// TODO: Implement a proper way to delete the string table
		// delete &file->m_strings;

		// TODO: Implement destructor for CRdrFile
		// delete file;
	}

	return 0;
}

bool CRdrFile::ValidateFormat()
{
	CBufferIO* file = NULL;
	char symbol = '\0';
	bool validated = false;
	bool endline = false;
	bool comment = false;
	s32 index = 0;
	
	do
	{
		while (true)
		{
			file = fstack.front();

			// Reached end of file?
			if (!file->fread(&symbol, 1))
				break;

			validated = true;
			
			if (!validated)
			{
				file->fseek(0, SEEK_SET);
				return index == 0;
			}

			// Tag begin
			if (symbol == '(')
				if (!endline && !comment) index++;

			// Tag end
			if (symbol == ')')
				if (!endline && !comment) index--;

			// New-line
			if (symbol == '\n')
				if (!endline) comment = false;

			// Carriage return
			if (symbol == '\r')
				if (!endline) comment = false;

			// Comment
			if (symbol == ';')
				if (!endline) comment = true;

			// Include path
			if (symbol == '\"' && !comment)
				endline = (bool)(endline ^ 1);
		}

		if (fstack.size() == 1)
		{
			validated = false;
			file->fseek(0, SEEK_SET);
			return index == 0;
		}

		fstack.pop(true);
	} while (true);
}

CRdrFile* CRdrFile::Load(zar::CZAR* archive, zar::CKey* key)
{
	if (!key)
		return NULL;

	if (key->GetSize() == -1)
		return NULL;

	size_t keySize = key->GetSize();

	void* zrdr_buf = zmalloc(keySize);

	if (!archive->Fetch(key, zrdr_buf, keySize))
		return NULL;

	CRdrFile* rdrFile = new CRdrFile();
	CSTable stable = CSTable(0, 1024);

	rdrFile->m_strings = stable;
	rdrFile->m_buffer = static_cast<char*>(zrdr_buf);
	rdrFile->m_size = keySize;
	rdrFile->type = ZRDR_ARRAY;
	rdrFile->array = NULL;
	rdrFile->Resolve(false);

	return rdrFile;
}

bool CRdrFile::Resolve(bool resolveA)
{
	_zrdr* header = NULL;
	_zrdr* resolved = NULL;
	char* start = NULL;
	char* str = NULL;
	char* table = NULL;

	header = reinterpret_cast<_zrdr*>(m_buffer);

	if (!header)
		return false;

	// Legacy version of zReader supported by the following games:
	// - Top Gun Hornet's Nest
	// - MechWarrior 3
	// - MechWarrior 3 Pirate's Moon
	// - Recoil
	// - Crimson Skies
	if (resolveA)
	{
		str = reinterpret_cast<char*>(&header[1]);
		resolved = reinterpret_cast<_zrdr*>(m_buffer[header->integer]);

		if (this->type != ZRDR_STRING && this->type == ZRDR_ARRAY)
		{
			for (u32 i = 1; i < this->array->integer; i++)
				_resolveA(&this->array[i], resolved, str);
			
			return true;
		}

		this->string = this->string - (s32)resolved;
		return true;
	}
	
	str = reinterpret_cast<char*>(header + 1);
	resolved = reinterpret_cast<_zrdr*>(m_buffer + header->integer);

	if (type == ZRDR_STRING)
		this->string = str;

	if (type != ZRDR_ARRAY)
	{
		start = m_buffer;
		table = NULL;

		if (start)
			table = start + sizeof(_zrdr);

		m_strings.LoadTable(table, reinterpret_cast<u32>(start), false);
		return true;
	}

	this->array = resolved;

	for (u32 i = 1; i < this->array->integer; i++)
		_resolveB(&this->array[i], resolved, str);

	start = m_buffer;

	table = NULL;

	if (start)
		table = start + sizeof(_zrdr);

	m_strings.LoadTable(table, *(u32*)start, false);
	return true;
}