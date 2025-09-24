#include "zar.h"
#include "Apps/FTS/gamever.h"

#include "gamez/zutil/util_stable.h"
#include "gamez/zutil/util_systemio.h"

namespace zar
{
	bool CZAR::Fetch(CKey* key, void* buf, size_t size)
	{
		CKey* openKey = NULL;

		if (!m_pFile)
			return false;

		openKey = OpenKey(key);

		if (!openKey)
			return false;

		if (!m_pFile->IsOpen())
			return false;

		if (key->m_size == 0)
		{
			CloseKey(key);
			return false;
		}

		GameZ_FTSGame game = GetGame();

		if (game == game_SOCOM1_BETA)
		{
			s32 offset = key->m_offset;
			size_t position = m_pFile->fseek(offset, SEEK_SET);

			if (offset == position && size <= key->m_size)
			{
				offset = m_pFile->fread(buf, size);
				ZAR_SECURE(m_bSecure, buf, size);

				return size == offset;
			}
		}
		else if (game == game_SOCOM1 || game == game_SOCOM2_BETA)
		{
			s32 offset = key->m_offset;
			size_t position = m_pFile->fseek(m_rootOffset + offset, SEEK_SET);

			if (m_rootOffset + offset == position && size <= key->m_size)
			{
				offset = m_pFile->fread(buf, size);
				ZAR_SECURE(m_bSecure, buf, size);

				return size == offset;
			}
		}

		return false;
	}

	bool CZAR::Fetch(const char* name, s32(*callback)(CZAR*, s32, void*), void* buf)
	{
		CKey* key = NULL;

		key = GetOpenKey();

		if (!key)
			return false;

		key = key->FindKey(name);

		if (!m_pFile)
			return false;

		CKey* bufferKey = OpenKey(key);

		if (!m_pFile->IsOpen() || !bufferKey)
			return false;

		if (m_pFile->fseek(key->m_offset, SEEK_SET) == -1)
			return false;

		hackStr = key->m_name;

		s32 result = callback(this, key->m_size, buf);

		if (result > 0 && result <= key->m_size)
		{
			CloseKey(key);
			return true;
		}

		CloseKey(key);
		return false;
	}

	bool CZAR::Fetch(const char* name, void* buf, size_t size)
	{
		s32 offset, position = 0;
		CKey* key = NULL;
		CKey* bufferKey = NULL;

		key = GetOpenKey();

		if (!key)
			return false;

		if (!m_pFile)
			return false;

		if (!m_pFile->IsOpen())
			return false;

		key = key->FindKey(name);
		bufferKey = OpenKey(key);

		if (!bufferKey)
			return false;

		if (key->m_size == 0)
		{
			CloseKey(key);
			return false;
		}

		offset = key->m_offset;
		position = m_pFile->fseek(offset, SEEK_SET);

		if (offset == position && size <= key->m_size)
		{
			offset = m_pFile->fread(buf, size);
			Unsecurify(buf, size);

			if (size == offset)
			{
				CloseKey(key);
				return true;
			}
		}

		CloseKey(key);
		return false;
	}

	bool CZAR::Fetch(const char* name, char** array)
	{
		size_t size = 0;
		char* strArray = NULL;

		if (!m_pFile)
			return false;

		if (!m_pFile->IsOpen())
			return false;

		size = GetSize(name);

		if (size <= 0)
			return false;

		strArray = (char*)calloc(size + 1, 1);
		*array = strArray;

		CKey* key = GetOpenKey();

		if (!key)
			return false;

		key = key->FindKey(name);
		//return Fetch(key, strArray, keySize, 0);
		return true;
	}

	bool CZAR::Fetch(const char* name, f32* buf)
	{
		CKey* key = GetOpenKey();

		if (!key)
			return false;

		key = key->FindKey(name);
		return Fetch(key, buf, sizeof(f32));
	}

	bool CZAR::Fetch(const char* name, u32* buf)
	{
		CKey* key = GetOpenKey();

		if (!key)
			return false;

		key = key->FindKey(name);
		return Fetch(key, buf, sizeof(u32));
	}

	bool CZAR::Fetch(const char* name, s32* buf)
	{
		CKey* key = GetOpenKey();

		if (!key)
			return false;

		key = key->FindKey(name);
		return Fetch(key, buf, sizeof(f32));
	}

	bool CZAR::Fetch(const char* name, bool* buf)
	{
		CKey* key = GetOpenKey();
		s8 cBool = false;

		if (!key)
			return false;

		key = key->FindKey(name);

		if (Fetch(key, &cBool, sizeof(s8)))
			*buf = cBool != 0;

		return true;
	}

	bool CZAR::FetchAll(s32(*callback)(CZAR*, char*, void*, s32, void*), void* buf)
	{
		s32 count = 0;
		bool valid = false;
		CKey* key = GetOpenKey();

		if (!m_pFile)
			return false;

		if (!m_pFile->IsOpen())
			return false;

		for (auto i = key->begin(); i != key->end(); i++)
		{
			CKey* current = *i;

			CKey* bufferKey = OpenKey(current);

			if (!bufferKey)
			{
				CloseKey(current);
				continue;
			}

			if (m_pFile->fseek(key->m_offset, SEEK_CUR) == -1)
				continue;

			void* listPointers = NULL;
			FetchLIP(current, &listPointers);
			s32 result = callback(this, current->m_name, listPointers, current->m_size, buf);

			if (result > 0)
				valid = result <= current->m_size;

			CloseKey(current);

			if (valid)
				count++;
		}

		return count != 0;
	}

	bool CZAR::FetchLIP(CKey* key, void** buf)
	{
		bool success = false;
		bool isOpen = false;

		if (!m_pFile)
			return false;

		if (!m_pFile->IsOpen())
			return false;

		CKey* openKey = OpenKey(key);

		if (!openKey)
			return false;

		if (key->m_size == 0)
		{
			CloseKey(key);
			return false;
		}

		s32 offset = key->m_offset;

		if (offset == m_pFile->fseek(offset, SEEK_SET))
		{
			if (m_pFile->fread(key->m_size, buf) != 0)
			{
				CloseKey(key);
				return true;
			}
		}

		CloseKey(key);
		return false;
	}

	size_t CZAR::FetchString(const char* name, char* buf, size_t size)
	{
		CKey* key = NULL;
		CKey* stringKey = NULL;
		u32 offset = 0;

		if (!m_pFile)
			return false;

		if (!m_pFile->IsOpen())
			return false;

		key = GetOpenKey();

		key = key->FindKey(name);

		if (!key)
			return 0;

		if (key->m_size > size)
			return 0;

		stringKey = OpenKey(key);

		if (!stringKey)
			return 0;

		if (key->m_size == 0)
		{
			CloseKey(key);
			return 0;
		}

		s32 position = m_pFile->fseek(key->m_offset, SEEK_SET);

		if (offset == position && stringKey->m_size <= key->m_size)
		{
			keyOffset = m_pFile->fread(buf, key->m_size);
			Unsecurify(buf, key->m_size);

			if (key->m_size == offset)
			{
				CloseKey(key);
				return key->m_size;
			}
		}

		CloseKey(key);
		return 0;
	}
}
