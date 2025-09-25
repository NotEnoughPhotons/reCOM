#ifndef ZIMGUI_H
#define ZIMGUI_H
#include <imgui/imgui.h>

#include "gamez/zSystem/zsys.h"

class CZIMGUI
{
public:
    CZIMGUI() = delete;
    
    static void Init();
    static void Shutdown();

    static bool Tick(f32 dT);
    static bool Tick_TextureDisplay(f32 dT);
    static bool Tick_SoundDisplay(f32 dT);
    static bool Tick_ReaderDisplay(f32 dT);
    static bool Tick_ZARDisplay(f32 dT);

    static ImGuiIO m_io;
};
#endif