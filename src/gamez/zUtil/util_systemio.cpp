#include <sys/stat.h>

#include "util_systemio.h"

size_t CIO::fwrite(const char* buf)
{
	size_t size = 0;

	if (buf == NULL)
	{
		size = 0;
	}
	else
	{
		size = strlen(buf);
	}

	size_t bytesWritten = fwrite(&size, 4);

	if (buf != NULL)
	{
		size_t bufBytesWritten = fwrite(buf, size);
		bytesWritten += bufBytesWritten;
	}

	return bytesWritten;
}

CFileIO::CFileIO()
{
	m_file = NULL;
	m_mode = 0;
	m_filesize = 0;
	m_cd = NULL;
}

CFileIO::CFileIO(const CFileIO& other)
{
	m_file = NULL;
	m_filesize = other.m_filesize;
	m_file = other.m_file;
	m_mode = other.m_mode;
	m_cd = NULL;
}

bool CFileIO::Open(const char* file, const char* mode)
{
	m_file = (s64)std::fopen(file, mode);

	if (m_file != NULL)
	{
		std::fseek((FILE*)m_file, 0, SEEK_END);
		m_filesize = std::ftell((FILE*)m_file);
		std::fseek((FILE*)m_file, 0, SEEK_SET);
	}

	return m_file != NULL;
}

void CFileIO::Close()
{
	bool open = m_file != NULL;

	if (open)
	{
		std::fclose((FILE*)m_file);
		m_file = NULL;
	}
}

void CFileIO::Release()
{
	m_file = NULL;
}

unsigned int CFileIO::GetMode() const
{
	return m_mode;
}

size_t CFileIO::GetSize() const
{
	return m_filesize;
}

bool CFileIO::IsOpen() const
{
	return -1 < m_file;
}

size_t CFileIO::fread(void* buf, size_t size)
{
	if (size == 0)
	{
		return 0;
	}
	
	return std::fread(buf, 1, size, (FILE*)m_file);
}

size_t CFileIO::fread(int offset, void** buf)
{
	*buf = NULL;
	return 0;
}

size_t CFileIO::fseek(int offset, int origin)
{
	if (origin != SEEK_SET && origin == SEEK_END)
	{
		offset += m_filesize;
	}
	
	return std::fseek((FILE*)m_file, offset, SEEK_SET);
}

size_t CFileIO::ftell()
{
	// Before this used to return 0
	// I changed it since it would be more -
	// useful.
	return std::ftell((FILE*)m_file);
}

size_t CFileIO::fwrite(const char* buf)
{
	size_t length = 0;

	if (buf == NULL)
	{
		return 0;
	}
	else
	{
		length = strlen(buf);
	}

	size_t writeBufResult = fwrite(&length, sizeof(char*));
	size_t writeStrResult = fwrite(buf, length);
	return writeStrResult + writeBufResult;
}

size_t CFileIO::fwrite(const void* buf, size_t count)
{
	return std::fwrite(buf, 1, count, (FILE*)m_file);
}

void CFileIO::fflush()
{

}

