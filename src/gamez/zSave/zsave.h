#ifndef ZSAVE_H
#define ZSAVE_H
#include <list>

#include "gamez/zArchive/zar.h"
#include "gamez/zReader/zrdr.h"

class CSaveModule;

class CSaveManager : public std::list<CSaveModule*>
{
public:
	class CSaveHeader
	{

	};

	CSaveManager(CIO*);

	bool GetHeader(const char*, s32, CSaveHeader*);

	bool Load(const char*, s32);
	bool Save(const char*, s32, s32, char*);
	bool SaveRdr(zar::CKey*, const char*, CRdrFile*);
	
	void RegisterModule(CSaveModule*);

	zar::CZAR* m_pZar;
	CIO* m_pFile;
	s32 m_TempBufferSize;
	char* m_TempBuffer;
	bool m_AbortLoad;
	bool m_AbortSave;
	char* m_Filename;
};

class CSaveModule
{
public:
	CSaveModule(const char*, CSaveManager*);

	bool SaveRdr(const char*, CRdrFile*);
	bool TestFlags(const s32 flags);

	void UnSerialize(const char*, zar::CZAR*, s32);
	void UnSerialize(const char*, CRdrFile*, s32);

	CSaveManager* m_Manager;
	char* m_ModuleName;
	s32 m_Priority;
	s32 m_Flags;
	zar::CKey* m_pKey;
};
#endif