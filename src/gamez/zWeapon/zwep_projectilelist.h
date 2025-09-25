#ifndef ZWEAPON_PROJECTILELIST_H
#define ZWEAPON_PROJECTILELIST_H
#include "vector"

#include "gamez/zIntersect/zintersect.h"
#include "gamez/zSystem/zsys_primitive.h"

class CZProjectile;

class CZProjectileList
{
public:
    bool GetNextDirectFireDI(zdb::DiIntersect** di, s32* free_handle);
    bool GetNextIndirectFireDI(zdb::DiIntersect** di, s32* handle);
	
    CZProjectile* m_projectiles;
    std::vector<CZProjectile*> m_explosives;
	
    u8* m_used;

    s32 m_firstfree;
    s32 m_lastused;
    s32 m_size;

    zdb::DiIntersect* m_directFireDI[60];
    u8 m_directFireDIUsed[60];
    s32 m_direct_firstfree;

    zdb::DiIntersect* m_indirectFireDI[30];
    u8 m_indirectFireDIUsed[60];
    s32 m_indirect_firstfree;
};
#endif