#ifndef ZVEHICLE_H
#define ZVEHICLE_H
class CVehicleRdr
{

};

class CVehicleRdrEntry
{
private:
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
#endif