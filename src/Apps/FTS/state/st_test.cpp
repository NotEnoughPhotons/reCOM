#define STB_IMAGE_IMPLEMENTATION
#include "Apps/FTS/hud/hud.h"

#include <SDL3_image/SDL_image.h>

#include "gamez/zNode/node_model.h"
#include "gamez/zNode/node_world.h"
#include "gamez/zIMGUI/zimgui.h"
#include "gamez/zVideo/zvid.h"

zdb::CNode* mdl = NULL;

bool LoadWorld(const char* name);

CTestState::CTestState()
{
    m_name = zstrdup("TestState");
}

bool CTestState::Init()
{
    LoadWorld("m8");
    mdl = zdb::CWorld::m_world->FindChild("halftribuild_2", true);
    mdl->SetActive(true);
    thePipe.m_camera = zdb::CWorld::m_world->m_camera;
    return true;
}

void CTestState::Tick(f32 dT)
{
    SDL_SetRenderDrawColor(theWindow->GetRenderer(), 15, 15, 15, 255);
    SDL_RenderClear(theWindow->GetRenderer());
    thePipe.RenderNode(mdl, zdb::tag_ZVIS_FOV::ZVIS_FOV_ALL_IN);
    SDL_RenderPresent(theWindow->GetRenderer());
}