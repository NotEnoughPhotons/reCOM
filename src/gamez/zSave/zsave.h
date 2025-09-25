#ifndef ZSAVE_H
#define ZSAVE_H
#include "gamez/zArchive/zar.h"
#include "gamez/zSystem/zsys.h"

#define TYPE_TEXTURES 1
#define TYPE_MODELS   2
#define TYPE_PALETTES 4

namespace zdb
{
    class CAssetLib;
    
    class CNode;
    class CWorld;
}

namespace zdb
{
    class CSaveLoad
    {
        friend class CNode;
    public:
        CSaveLoad();

        CWorld* Load(const char* path);
        bool LoadAssetLib(CWorld* world, CAssetLib* library, u32 type);
        bool LoadAssetLib_PS2(CWorld* world, CAssetLib* library, u32 type);
        bool LoadPalettes_PS2(CAssetLib* library) { return false; } // TODO: Not implemented
        bool LoadTextures_PS2(CAssetLib* library);
    public:
        CWorld* m_world;
        zar::CZAR m_zfile;
        char m_zed_filename[1024];
        s32 m_version;
    };
}
#endif