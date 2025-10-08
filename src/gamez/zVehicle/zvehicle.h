﻿#ifndef ZVEHICLE_H
#define ZVEHICLE_H
#include <vector>

#include "gamez/zAI/zai.h"
#include "gamez/zEntity/zentity.h"

class CVehicleRdrEntry;

class CVehicleRdr : public std::vector<CVehicleRdrEntry>
{
public:
    bool Open(const char*);
    void Close();
    bool Load(_zrdr*);
    CVehicleRdrEntry* GetEntryByName(const char*);
};

class CVehicleRdrEntry
{
public:
    void SetCharacterName(const char*);

    char m_name[32];
    char m_character[48];
    char m_display_name[32];
    startvec m_start;
    s32 m_teamMask;
    AI_PARAMS m_aiParams;
    u32 m_debug : 1;
    u32 m_disabled : 1;
    u32 m_nofade : 1;
    u32 m_noshoot : 1;
    u32 m_recycle : 1;
    u32 m_nosnooze : 1;
    u32 m_unused : 26;
    s32 m_startIndex;
};

extern CVehicleRdrEntry* _dest;
#endif