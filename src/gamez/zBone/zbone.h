#ifndef ZBONE_H
#define ZBONE_H
#include "gamez/zArchive/zar.h"

bool zBoneInit();
bool zBoneOpen();
bool zBoneLoadAll();
bool zBoneClose();

class CScriptArchive : public zar::CZAR
{
public:
    
};

extern u32 bone_archive_version;
extern CScriptArchive boneArchive;
extern size_t Sizeof_MotionRdr;
extern void* pBuffer_MotionRdr;
#endif