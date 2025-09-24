#include <SDL3/SDL_events.h>

#include "gamez/zFTS/zfts.h"
#include "gamez/zGame/zgame.h"
#include "gamez/zSeal/zseal.h"
#include "gamez/zRender/zrender.h"
#include "gamez/zVideo/zvid.h"

CCoreState::CCoreState()
{
	m_name = zstrdup("CoreState");
	m_loading = false;
	m_loadstarted = false;
	// objMtx1 = CMatrix::identity;
	// objMtx2 = CMatrix::identity;
	m_skip_swap = 0;
}

bool CCoreState::Init()
{
	CZSealBody* seal = NULL;

	m_world = zdb::CWorld::m_world;

	zdb::CVisual::m_camera = m_world->m_camera;
	thePipe.m_camera = zdb::CVisual::m_camera;

	m_hud = new CHUD();
	theHUD = m_hud;

	if (gopt.doHud != false)
	{
		m_hud->Init(m_world);
		CameraZoom::AttachHud(m_hud);
	}

	seal = ftsGetPlayer();
	CameraZoom::AttachSeal(seal);
	CRdrArchive::CloseAll();
	CRdrArchive::RemoveArchive("readerm.zar", "database");

	zTaskScheduler.AddTask("UnitTick", CSealUnit::TickAll, 0.8f, NULL);

	theMission.OnMissionStart();
	theMission.m_winlosstimer = -1.0f;

	zSysFifoEnd();
	CVideo::MaxFrameRate(CVideo::m_defaultFrameRate);
	zVid_Sync0();

	return true;
}

void CCoreState::Tick(f32 dT)
{
	SDL_Event e;
	SDL_PollEvent(&e);
	zVid_Swap(true);
}