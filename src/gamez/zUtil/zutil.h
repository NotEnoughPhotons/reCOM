#ifndef ZUTIL_H
#define ZUTIL_H
#include "util_systemio.h"

#include "gamez/zSystem/zsys_primitive.h"

namespace zar
{
	class CKey;
}

extern bool dumpLog;

class COutput;

extern COutput* debug;

void zUtilInit(const char* log);

s32 _countKeys(zar::CKey* key);

class COutput
{
public:
	COutput(const char* log, bool doLog);

	static void DumpLog(const char* file);
private:
	bool m_doLog;
	CFileIO* m_fileIO;
};
#endif