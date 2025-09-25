#include "zar.h"
#include "Apps/FTS/gamever.h"
#include "gamez/zUtil/zutil.h"
#include "SDL3/SDL_stdinc.h"

namespace zar
{
	CKey::CKey()
	{
		m_name = NULL;
		m_size = 0;
		m_offset = 0;
	}

	CKey::CKey(char* name)
	{
		m_name = name;
		m_size = 0;
		m_offset = 0;
	}

	CKey::~CKey()
	{
		clear();
	}

	CKey* CKey::InsertKey(CKey* key)
	{
		insert(begin(), key);
		return key;
	}

	CKey* CKey::FindKey(const char* name)
	{
		for (auto i = begin(); i != end(); i++)
		{
			CKey* current = *i;

			if (current->m_size != -1 && SDL_strcasecmp(current->m_name, name) == 0)
				return current;
		}

		return NULL;
	}

	bool CKey::Read(CZAR* archive, CBufferIO* buffer, s64 stableOffset)
	{
		struct KEY
		{
			s32 m_name;
			u32 m_offset;
			u32 m_size;
			u32 m_count;
		} key_t;

		buffer->fread(&key_t, sizeof(KEY));

		if (archive->m_root != this)
		{
			m_name = (char*)key_t.m_name + stableOffset;
			m_size = key_t.m_size;
			m_offset = key_t.m_offset;
		}
		
		while (key_t.m_count != 0)
		{
			CKey* archiveKey = archive->NewKey(NULL);
			archiveKey->Read(archive, buffer, stableOffset);
			insert(end(), archiveKey);
			key_t.m_count--;
		}

		return true;
	}

	bool CKey::Write(CZAR* archive)
	{
		archive->Securify(m_name, 4);
		archive->m_pFile->fwrite(m_name, 4);
		archive->Unsecurify(m_name, 4);

		archive->Securify(&m_offset, 4);
		archive->m_pFile->fwrite(&m_offset, 4);
		archive->Unsecurify(&m_offset, 4);

		archive->Securify(&m_size, 4);
		archive->m_pFile->fwrite(&m_size, 4);
		archive->Unsecurify(&m_size, 4);

		archive->Securify(this, 4);
		archive->m_pFile->fwrite(this, 4);
		archive->Unsecurify(this, 4);

		for (auto it = begin(); it != end(); ++it)
		{
			CKey* key = *it;
			key->Write(archive);
		}

		return true;
	}

	bool CKey::Write(CZAR* archive, FILE* file)
	{
		archive->Securify(m_name, 4);
		fwrite(m_name, 1, sizeof(s32), file);
		archive->Unsecurify(m_name, 4);

		archive->Securify(&m_offset, 4);
		fwrite(&m_offset, 1, sizeof(s32), file);
		archive->Unsecurify(&m_offset, 4);

		archive->Securify(&m_size, 4);
		fwrite(&m_size, 1, sizeof(s32), file);
		archive->Unsecurify(&m_size, 4);

		archive->Securify(this, 4);
		fwrite(this, 1, sizeof(s32), file);
		archive->Unsecurify(this, 4);

		for (auto i = begin(); i != end(); ++i)
		{
			CKey* key = *i;
			key->Write(archive, file);
		}

		return true;
	}

	void CKey::fixupKey(CSTable* table, CKey* key)
	{
		char* keyname = table->FindString(key->m_name);
		key->m_name = keyname;

		auto key_iterator = key->begin();

		for (auto i = key->begin(); i != key->end(); i++)
		{
			CKey* child = *key_iterator;

			char* childname = table->FindString(child->m_name);
			child->m_name = childname;

			for (auto j = child->begin(); j != child->end(); j++)
				fixupKey(table, *j);
		}
	}

	CZAR::CZAR(const char* name, CIO* io)
	{
		m_pFileAlloc = NULL;
		m_databuffer = NULL;
		m_root = NULL;
		m_stable = NULL;
		m_filename = NULL;
		m_bCompress = false;
		m_bSecure = false;

		memset(&m_tail, 0, sizeof(TAIL));

		m_tail.version = 0x20001;

		m_stable = new CSTable(0, 1024);

		if (m_filename != "DEFAULT_ZAR_NAME")
		{
			if (m_filename == name)
			{
				if (name == "DEFAULT_ZAR_NAME")
					m_filename = "DEFAULT_ZAR_NAME";
				else
					m_filename = strdup(name);

				if (!io)
					m_pFileAlloc = new CFileIO();

				m_pFile = new CBufferIO();

				m_data_padded = 16;

				m_root = new CKey("DEFAULT_ZAR_NAME");

				return;
			}

			free(m_filename);
			m_filename = NULL;
		}

		if (name == "DEFAULT_ZAR_NAME")
			m_filename = "DEFAULT_ZAR_NAME";
		else
			m_filename = strdup(name);

		if (io == NULL)
			m_pFileAlloc = new CFileIO();

		m_pFile = new CBufferIO();

		m_data_padded = 16;

		m_root = new CKey("DEFAULT_ZAR_NAME");
	}

	CZAR::~CZAR()
	{
		if (m_pFile != NULL)
		{
			if (m_pFile->IsOpen() && (m_pFile->GetMode() & 6) != 0)
			{

			}
		}
	}

	void CZAR::Close()
	{
		if (!m_pFile)
			return;

		if (m_pFile->IsOpen() && (m_pFile->GetMode() & 6) != 0)
		{
			if (m_modified)
			{
				WriteDirectory();
				m_modified = false;
			}
		}

		if (!m_pFileAlloc && m_pFile)
		{
			m_pFile->Close();
		}
		else
		{
			m_pFileAlloc->Close();
			delete m_pFileAlloc;
			m_pFileAlloc = NULL;
			m_pFile = NULL;
		}

		m_key_buffer.clear();

		m_root->erase(m_root->begin(), m_root->end());
	}

	void CZAR::Close(bool clearBuffer)
	{
		if (!m_pFileAlloc)
		{
			if (m_pFile)
				m_pFile->Close();
		}
		else
		{
			m_pFileAlloc->Close();
			delete m_pFileAlloc;
			m_pFileAlloc = NULL;
			m_pFile = NULL;
		}

		m_key_buffer.clear();

		m_root->erase(m_root->begin(), m_root->end());

		m_stable->Destroy();
	}

	void CZAR::CloseKeepDir()
	{
		m_pFile->Close();
	}

	CKey* CZAR::CreateKey(const char* name)
	{
		CKey* key = NULL;

		// TODO:
		// Redo this function definition

		return key;
	}

	CKey* CZAR::NewKey(const char* name)
	{
		CKey* key = NULL;

		if (!name)
		{
			m_key_buffer.insert(m_key_buffer.begin(), new CKey());
		}
		else
		{
			char* keyName = m_stable->CreateString(name);
			key = new CKey(keyName);
			m_key_buffer.insert(m_key_buffer.begin(), key);
		}

		key = m_key_buffer.front();

		return key;
	}

	CKey* CZAR::FindKey(const char* name)
	{
		CKey* key = NULL;

		if (m_keys.empty())
			key = m_root;
		else
			key = m_keys.front();

		if (key != NULL)
			return key->FindKey(name);

		return key;
	}

	CKey* CZAR::OpenKey(const char* keyName)
	{
		CKey* key = NULL;

		if (!m_pFile)
			return NULL;

		if (!m_pFile->IsOpen())
			return NULL;

		if (m_keys.empty())
			key = m_root;
		else
			key = m_keys.front();

		key = key->FindKey(keyName);

		if (key)
			m_keys.insert(m_keys.begin(), key);

		return key;
	}

	CKey* CZAR::OpenKey(CKey* key)
	{
		if (!key)
			return NULL;

		m_keys.insert(m_keys.begin(), key);
		return key;
	}

	CKey* CZAR::GetOpenKey() const
	{
		CKey* key;

		if (m_keys.empty())
			key = m_root;
		else
			key = m_keys.front();

		return key;
	}

	bool CZAR::Open(const char* name, s32 version, u32 mode, size_t padded_size)
	{
		if (!name)
			return false;

		if (!m_pFile)
		{
			m_pFileAlloc = new CFileIO();
			m_pFile = new CBufferIO();
		}

		if (!m_pFileAlloc->Open(name))
			return false;

		if (name && name == "DEFAULT_ZAR_NAME")
			m_filename = "DEFAULT_ZAR_NAME";
		else
			m_filename = strdup(name);

		if (m_filename 
			&& m_filename != "DEFAULT_ZAR_NAME" 
			&& m_filename == name)
		{
			m_data_padded = padded_size;

			bool success = false;

			if (GetGame() == game_SOCOM1_BETA)
				success = ReadDirectory(version, mode);
			else if (GetGame() == game_SOCOM1 || GetGame() == game_SOCOM2_BETA)
				success = ReadDirectory_V2(version, mode);

			if (success)
			{
				m_tail.appversion = version;
			}
			else
			{
				if (m_pFile->IsOpen() && m_modified)
				{
					WriteDirectory();
					m_modified = false;
				}

				if (!m_pFileAlloc)
				{
					m_pFile->Close();
				}
				else
				{
					m_pFileAlloc->Close();
					delete m_pFileAlloc;
					m_pFileAlloc = NULL;
					m_pFile = NULL;
				}

				m_key_buffer.clear();
				m_root->erase(m_root->begin(), m_root->end());
				m_stable->Destroy();
			}

			zfree(m_filename);
			m_filename = NULL;

			return success;
		}

		return false;
	}
	
	bool CZAR::ReOpen(s32 appver, s32 mode)
	{
		if (!m_pFileAlloc)
		{
			m_pFileAlloc = new CFileIO();
			m_pFile = new CBufferIO();
		}
		
		if (!m_pFileAlloc->IsOpen())
			return true;

		if (!m_filename)
			return false;

		if (m_pFileAlloc->Open(m_filename))
		{
			SetFilename(m_filename);
			m_data_padded = 16;

			if (GetGame() == game_SOCOM1_BETA)
			{
				if (ReadDirectory(appver, mode)) m_tail.appversion = appver;
				else Close();
			}
			else if (GetGame() == game_SOCOM1 || GetGame() == game_SOCOM2_BETA)
			{
				if (ReadDirectory_V2(appver, mode)) m_tail.appversion = appver;
				else Close();
			}
		}

		return false;
	}
	
	void CZAR::CloseKey(CKey* key)
	{
		if (key && !m_keys.empty())
			m_keys.erase(m_keys.begin());
	}

	bool CZAR::Insert(zar::CKey* key, void* buf, size_t size)
	{
		s32 keySize, writeOffset, position, keyPad = 0;
		char padChar = 0;
		bool validPosition, validOffset = false;

		if (!buf)
			return false;

		if (size <= 0)
			return false;

		keySize = size;

		position = m_pFile->fseek(0, SEEK_END);
		validPosition = position != -1;

		if (!validPosition)
			return false;

		if (!m_bSecure)
		{
			writeOffset = m_pFile->fwrite(buf, size);
		}
		else
		{
			void* arrBuf = calloc(1, size);
			void* dest = arrBuf;
			memcpy(dest, buf, size);

			ZAR_SECURE(m_bSecure, buf, size)
		}

		validOffset = size == writeOffset;

		if (!validOffset)
			return false;

		key->m_offset = position;
		key->m_size = size;

		keyPad = size % m_data_padded;
		keySize = 0;

		if (keyPad != 0)
			keySize = m_data_padded - keyPad;

		padChar = '�';

		if (!m_bSecure)
			padChar = 'P';

		for (s32 i = 0; i < keySize; i++)
			m_pFile->fwrite(&padChar, 1);

		return true;
	}

	CKey* CZAR::Insert(const char* name, void* buf, size_t size)
	{
		CKey* newKey = NewKey(name);
		CKey* openKey = GetOpenKey();

		bool insertionSuccess = Insert(newKey, buf, size);

		if (!insertionSuccess && newKey)
		{
			delete newKey;
			newKey = NULL;
		}
		else
		{
			openKey->InsertKey(newKey);
		}

		return newKey;
	}

	CKey* CZAR::Insert(const char* name, u32 value)
	{
		CKey* newKey = NewKey(name);
		CKey* openKey = GetOpenKey();

		bool insertionSuccess = Insert(newKey, &value, sizeof(value));

		if (!insertionSuccess && newKey)
		{
			delete newKey;
			newKey = NULL;
		}
		else
		{
			openKey->InsertKey(newKey);
		}

		return newKey;
	}

	CKey* CZAR::Insert(const char* name, s32 value)
	{
		CKey* newKey = NewKey(name);
		CKey* openKey = GetOpenKey();

		bool insertionSuccess = Insert(newKey, &value, sizeof(value));

		if (!insertionSuccess && newKey)
		{
			delete newKey;
			newKey = NULL;
		}
		else
		{
			openKey->InsertKey(newKey);
		}

		return newKey;
	}

	bool CZAR::ReadDirectory(s32 appver, u32 mode)
	{
		CBufferIO bufferIO;
		size_t key_size = 0;

		void* stable_ptr = NULL;
		void* key_ptr = NULL;
		
		// No file memory allocator?
		if (!m_pFileAlloc)
			return false;

		// Is the file open?
		if (!m_pFileAlloc->IsOpen())
			return false;

		// Reached end of file or corrupted?
		if (m_pFileAlloc->fseek(-96, SEEK_END) == -1)
			return false;

		// Is the ZAR header valid?
		if (m_pFileAlloc->fread(&m_tail, sizeof(TAIL)) != sizeof(TAIL))
			return false;

		// Is this ZAR version compatible?
		if (m_tail.version != ZAR_VERSION_1)
			return false;

		// Is there an issue with going to the start of the file?
		if (m_pFileAlloc->fseek(m_tail.offset, SEEK_SET) == -1)
			return false;

		stable_ptr = malloc(m_tail.stable_size);

		// Read key names into memory.
		m_pFileAlloc->fread(stable_ptr, m_tail.stable_size);

		// If a table already exists, destroy it.
		// Populate the new table with our strings.
		m_stable->Destroy();
		m_stable->LoadTable(stable_ptr, m_tail.stable_size, true);

		// Clean out the key buffers and prepare them for new keys.
		// A reservation here is made to reduce future allocations.
		m_key_buffer.clear();
		m_key_buffer.reserve(m_tail.key_count);

		m_root->erase(m_root->begin(), m_root->end());

		key_size = (s64)m_tail.key_count << 4;
		key_ptr = malloc(key_size);

		// Begin reading key bytes into memory.
		// ZAR keys have the option of being "securified",
		// which means their data is scrambled.
		m_pFileAlloc->fread(key_ptr, key_size);
		Unsecurify(key_ptr, key_size);

		bufferIO.Open(key_ptr, key_size);

		// Keys can contain other keys. The root kickstarts the process of
		// adding its children to their keyring.
		// 
		// ZAR headers have a field to keep track of where the string table
		// should be put in memory. (m_tail.stable_ofs)
		// 
		// The field is then subtracted from where the string table is allocated,
		// which is then used as an input here to retrieve what the key's name is in memory.
		m_root->Read(this, &bufferIO, (s64)stable_ptr - m_tail.stable_ofs);
		bufferIO.Close();

		m_databuffer_size = m_tail.offset;

		if (m_databuffer_size != 0)
		{
			m_databuffer = malloc(m_databuffer_size);

			m_pFileAlloc->fseek(0, SEEK_SET);
			m_pFileAlloc->fread(m_databuffer, m_databuffer_size);
			Unsecurify(m_databuffer, m_databuffer_size);

			if (m_pFileAlloc != NULL)
			{
				m_pFileAlloc->Close();
				delete m_pFileAlloc;

				m_pFile = NULL;
				m_pFileAlloc = NULL;

				m_pFileAlloc = new CFileIO();
				m_pFile = new CBufferIO();

				m_pFile->Open(m_databuffer, m_databuffer_size);
			}
		}

		return true;
	}

	bool CZAR::ReadDirectory_V2(s32 appver, u32 mode)
	{
		CBufferIO bufferIO;
		s32 padding, key_ofs = 0;
		size_t key_size, header_size = 0;

		void* stable_ptr = NULL;
		void* key_ptr = NULL;
		
		// No file memory allocator?
		if (!m_pFileAlloc)
			return false;

		// Is the file open?
		if (!m_pFileAlloc->IsOpen())
			return false;

		// Is the file corrupted/invalid?
		if (m_pFileAlloc->fseek(0, SEEK_SET) == -1)
			return false;

		header_size = m_pFileAlloc->fread(&m_head, sizeof(HEAD));

		if (header_size != sizeof(HEAD))
			return false;

		// Deobfuscate the header.
		Unsecurify(&m_head, sizeof(HEAD));
		
		// Is this ZAR version compatible?
		if (m_head.version != ZAR_VERSION_2)
			return false;

		stable_ptr = zmalloc(m_head.stable_size);

		// Read key names into memory.
		m_pFileAlloc->fread(stable_ptr, m_head.stable_size);

		// Deobfuscate the string table contents.
		Unsecurify(stable_ptr, m_head.stable_size);

		// If a table already exists, destroy it.
		// Populate the new table with our strings.
		m_stable->Destroy();
		m_stable->LoadTable(stable_ptr, m_head.stable_size, true);

		// Clean out the key buffers and prepare them for new keys.
		// A reservation here is made to reduce future allocations.
		m_key_buffer.clear();
		m_key_buffer.reserve(m_head.key_count);

		m_root->erase(m_root->begin(), m_root->end());

		key_size = (s64)m_head.key_count << 4;
		key_ptr = malloc(key_size);

		// Begin reading key bytes into memory.
		// ZAR keys have the option of being "securified",
		// which means their data is scrambled.
		m_pFileAlloc->fread(key_ptr, key_size);
		Unsecurify(key_ptr, key_size);

		bufferIO.Open(key_ptr, key_size);

		// Keys can contain other keys. The root kickstarts the process of
		// adding its children to their keyring.
		// 
		// ZAR headers have a field to keep track of where the string table
		// should be put in memory. (m_tail.stable_ofs)
		// 
		// The field is then subtracted from where the string table is allocated,
		// which is then used as an input here to retrieve what the key's name is in memory.
		m_root->Read(this, &bufferIO, (s64)stable_ptr - m_head.stable_ofs);
		bufferIO.Close();

		// Calculate the alignment per key.
		// This is a new field exclusive to SOCOM 1 and beyond.
		padding = m_head.padding;
		key_ofs = key_size + m_head.stable_size + sizeof(HEAD);
		s32 align = key_ofs % padding;

		padding = !align ? 0 : padding - align;

		m_pFileAlloc->fseek(padding, SEEK_CUR);

		if ((mode & 32) == 0)
		{
			if (header_size != 0)
			{
				m_databuffer_size = m_head.offset;
				m_databuffer = zmalloc(m_databuffer_size);
				m_rootOffset = padding + key_ofs;
				m_pFileAlloc->fseek(m_rootOffset, SEEK_CUR);
				m_pFileAlloc->fread(m_databuffer, m_databuffer_size);
				Unsecurify(m_databuffer, m_databuffer_size);

				if (m_pFileAlloc != NULL)
				{
					m_pFileAlloc->Close();
					delete m_pFileAlloc;

					m_pFile = NULL;
					m_pFileAlloc = NULL;

					m_pFileAlloc = new CFileIO();
					m_pFile = new CBufferIO();

					m_pFile->Open(m_databuffer, m_databuffer_size);
					m_rootOffset = 0;
				}
			}
		}
		else
		{
			m_rootOffset = padding + key_ofs;
		}

		return true;
	}

	bool CZAR::WriteDirectory()
	{
		bool success = false;

		if (!m_pFile)
			return false;

		if (!m_pFile->IsOpen())
			return false;

		u32 pos = m_pFile->fseek(0, SEEK_END);
		success = pos != -1;

		if (m_pFile->fseek(0, SEEK_END) == -1)
			return false;

		m_tail.version = ZAR_VERSION_1;
		m_tail.offset = m_pFile->ftell();
		m_tail.key_count = _countKeys(m_root);
		m_stable->Pack(CKey::fixupKey, m_root);
		m_tail.stable_size = m_stable->m_bytes;
		m_tail.stable_ofs = (s32)m_stable->m_buffer;

		if (!m_bSecure)
		{
			m_pFile->fwrite(m_stable->m_buffer, m_stable->m_bytes);
		}
		else
		{
			void* array = zcalloc(1, m_stable->m_bytes);
			memcpy(array, m_stable->m_buffer, m_stable->m_bytes);

			Securify(array, m_stable->m_bytes);

			m_pFile->fwrite(array, m_stable->m_bytes);

			zfree(array);
		}

		m_root->Write(this);
		Securify(&m_tail, sizeof(TAIL));

		m_pFile->fwrite(&m_tail, sizeof(TAIL));
		Unsecurify(&m_tail, sizeof(TAIL));

		m_modified = false;

		return true;
	}

	bool CZAR::WriteDirectory(FILE* file)
	{
		bool success = false;

		if (!file)
			return false;

		fwrite((void*)m_pFile->m_file, 1, m_pFile->m_filesize, file);

		if (fseek(file, 0, SEEK_END) == -1)
			return false;

		m_tail.version = ZAR_VERSION_1;
		m_tail.offset = ftell(file);
		m_tail.key_count = _countKeys(m_root);
		m_stable->Pack(CKey::fixupKey, m_root);
		m_tail.stable_size = m_stable->m_bytes;
		m_tail.stable_ofs = (s32)m_stable->m_buffer;

		if (!m_bSecure)
		{
			fwrite(m_stable->m_buffer, 1, m_stable->m_bytes, file);
			s32 pos = ftell(file);
			s32 align = pos % 16;
			s32 nextpos = 16 - align;
			char padding = '\0';

			for (u32 i = 0; i < nextpos; i++)
				fwrite(&padding, 1, 1, file);
		}
		else
		{
			void* array = zcalloc(1, m_stable->m_bytes);
			memcpy(array, m_stable->m_buffer, m_stable->m_bytes);
			Securify(array, m_stable->m_bytes);
			m_pFile->fwrite(array, m_stable->m_bytes);
			zfree(array);
		}

		m_root->Write(this, file);
		Securify(&m_tail, sizeof(TAIL));
		fwrite(&m_tail, 1, sizeof(TAIL), file);
		Unsecurify(&m_tail, sizeof(TAIL));

		m_modified = false;

		return success;
	}
	
	size_t CZAR::GetSize(const char* name)
	{
		CKey* key = NULL;

		if (m_keys.empty()) key = m_root;
		else key = m_root->front();

		key = key->FindKey(name);

		if (!key) return -1;
		else return key->m_size;

		return 0;
	}

	void* CZAR::ReleaseDataBuffer()
	{
		void* buffer = NULL;

		if (!m_databuffer)
			return NULL;
		
		CBufferIO* fileAlloc = dynamic_cast<CBufferIO*>(m_pFile);

		if (fileAlloc)
			fileAlloc->Release();

		buffer = m_databuffer;
		m_databuffer = NULL;

		return buffer;
	}

	void CZAR::SetFilename(const char* name)
	{
		if (m_filename && m_filename != "DEFAULT_ZAR_NAME")
		{
			if (name == m_filename)
				return;

			zfree(m_filename);
			m_filename = NULL;
		}

		if (!name || name == "DEFAULT_ZAR_NAME")
			m_filename = "DEFAULT_ZAR_NAME";
		else
			m_filename = zstrdup(name);
	}

	void CZAR::Securify(void* buf, size_t size)
	{
		ZAR_SECURE(m_bSecure, buf, size)
	}

	void CZAR::Unsecurify(void* buf, size_t size)
	{
		ZAR_SECURE(m_bSecure, buf, size)
	}
}
