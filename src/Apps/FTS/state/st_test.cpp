#define STB_IMAGE_IMPLEMENTATION
#include "Apps/FTS/hud/hud.h"

#include "GL/glew.h"

#include <SDL3_image/SDL_image.h>

#include "gamez/zNode/node_model.h"
#include "gamez/zNode/node_world.h"
#include "gamez/zIMGUI/zimgui.h"
#include "gamez/zVideo/zvid.h"

zdb::CNode* mdl = NULL;
C2DPoly* twodpoly = NULL;

bool LoadWorld(const char* name);

CTestState::CTestState()
{
    m_name = zstrdup("TestState");
}

bool CTestState::Init()
{
    CRdrFile* dbSettings = zrdr_read("./data/zrdr/test_level.rdr", NULL, ZRDR_FLAG_RAW);
    char* db = zrdr_findstring(dbSettings, "database");
    char* modelname = zrdr_findstring(dbSettings, "node");
    zrdr_free(dbSettings);

    LoadWorld(db);
    mdl = zdb::CWorld::GetModel(modelname);
    mdl->m_active = true;
    thePipe.m_camera = zdb::CWorld::m_world->m_camera;
    twodpoly = new C2DPoly();
    twodpoly->Load(-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, NULL);
    twodpoly->On();

    // glCullFace(GL_BACK);
    // glFrontFace(GL_CW);
    zgl_enable_ztest();
    // glDisable(GL_CULL_FACE);

    return true;
}

void CTestState::Tick(f32 dT)
{
    SDL_CaptureMouse(true);

    SDL_Event e;

    while (SDL_PollEvent(&e))
    {
        if (e.key.type == SDL_EVENT_QUIT)
            SDL_Quit();

        if (e.type == SDL_EVENT_MOUSE_MOTION)
            CMouse::ProcessMouseMotion(reinterpret_cast<SDL_MouseMotionEvent&>(e));

        if (e.type == SDL_EVENT_KEY_DOWN)
            CKeyboard::m_keys[e.key.scancode] = true;
        else
            CKeyboard::m_keys[e.key.scancode] = false;
    }

    zdb::CWorld::m_world->m_camera->Update(zdb::tag_ZCAM_TYPE::ZCAM_NORMAL);

    glViewport(0, 0, theWindow->GetWidth(), theWindow->GetHeight());
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    twodpoly->Draw(zdb::CWorld::m_world->m_camera);

    for (auto i = mdl->m_child.begin(); i != mdl->m_child.end(); ++i)
    {
        zdb::CNode* child = *i;
        child->SetActive(true);
        thePipe.RenderNode(child, zdb::tag_ZVIS_FOV::ZVIS_FOV_ALL_IN);
    }

    // thePipe.RenderNode(mdl, zdb::tag_ZVIS_FOV::ZVIS_FOV_ALL_IN);

    // thePipe.RenderWorld(zdb::CWorld::m_world);

    SDL_GL_SwapWindow(theWindow->GetWindow());
}