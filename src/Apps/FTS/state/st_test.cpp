#define STB_IMAGE_IMPLEMENTATION
#include "Apps/FTS/hud/hud.h"

#include "GL/glew.h"

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
    LoadWorld("ui");
    mdl = zdb::CWorld::m_world->GetModel("SatchelUl");
    mdl->SetActive(true);
    thePipe.m_camera = zdb::CWorld::m_world->m_camera;
    return true;
}

void CTestState::Tick(f32 dT)
{
    // SDL_SetRenderDrawColor(theWindow->GetRenderer(), 15, 15, 15, 255);
    SDL_RenderClear(theWindow->GetRenderer());

    glViewport(0, 0, 1280, 960);
    glClearColor(15, 15, 15, 255);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto i = mdl->m_child.begin(); i != mdl->m_child.end(); ++i)
    {
        zdb::CNode* child = *i;
        child->SetActive(true);
        thePipe.RenderNode(child, zdb::tag_ZVIS_FOV::ZVIS_FOV_ALL_IN);
    }

    SDL_RenderPresent(theWindow->GetRenderer());
    SDL_GL_SwapWindow(theWindow->GetWindow());

    SDL_Event e;

    while (SDL_PollEvent(&e))
    {

    }
}