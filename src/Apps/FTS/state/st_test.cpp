#define STB_IMAGE_IMPLEMENTATION
#include "hud.h"

#include <SDL3_image/SDL_image.h>

#include "gamez/zIMGUI/zimgui.h"
#include "gamez/zVideo/zvid.h"

CTestState::CTestState()
{
    m_name = zstrdup("TestState");
}

bool CTestState::Init()
{
    return true;
}

void CTestState::Tick(f32 dT)
{
    SDL_SetRenderDrawColor(theWindow->GetRenderer(), 15, 15, 15, 255);
    SDL_RenderClear(theWindow->GetRenderer());
    CZIMGUI::Tick(dT);
    SDL_RenderPresent(theWindow->GetRenderer());
}