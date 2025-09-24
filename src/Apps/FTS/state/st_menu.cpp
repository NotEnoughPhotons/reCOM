#include <SDL3/SDL_events.h>

#include "gamez/zCamera/zcam.h"
#include "gamez/zGame/zgame.h"
#include "gamez/zInput/zinput.h"
#include "gamez/zRender/zrender.h"
#include "gamez/zSystem/zsys.h"
#include "gamez/zVideo/zvid.h"

bool LoadWorld(const char* name);

CMenuState::CMenuState()
{
	m_name = zstrdup("MenuState");
	m_skip = 0;
	m_skip_swap = 0;

	TtySoundString = CSaferStr();
	BackSoundString = CSaferStr();

	SelectionSound = NULL;

	strncpy(m_DefaultMenu, "dlgIntroScreen.rdr", 64);

	TtySoundString.reserve(64);
	BackSoundString.reserve(64);
}

bool CMenuState::Init()
{
	CInput::Flush();

	if (m_skip == 0)
	{
		char pathbuf[64];
		sprintf_s(pathbuf, 64, "%s/ui", gamez_GameRunPath);
		
		zar::CZAR* archive = CRdrArchive::AddArchive("readerc.zar", pathbuf);

		if (archive)
		{
			archive->ReOpen(CRdrArchive::version, 1);
		}

		// zdb::CTexManager::m_texmanager->doSetupBuffers();
		
		LoadWorld("ui");

		if (zdb::CWorld::m_world)
		{
			zdb::CCamera* camera = zdb::CWorld::m_world->m_camera;
			
			camera->SetPosition(320.0f, 0.0f, 320.0f);
			camera->Update(zdb::tag_ZCAM_TYPE::ZCAM_NORMAL);
			zdb::CVisual::m_camera = camera;
			thePipe.m_camera = camera;
		}

		m_menu = new CGameMenu();

		CRdrFile* uisounds = zrdr_read("uisounds.rdr", "data/common/dialog");

		if (uisounds)
		{
			_zrdr* sounds = zrdr_findtag(uisounds, "SOUNDS");

			if (sounds)
			{
				char* teletype = zrdr_findstring(sounds, "TELETYPE");

				if (!teletype)
				{
					TtySoundString = NULL;
				}
				else
				{
					TtySoundString = teletype;
				}

				char* back = zrdr_findstring(sounds, "BACK");

				if (!back)
				{
					BackSoundString = NULL;
				}
				else
				{
					BackSoundString = back;
				}
			}

			zrdr_free(uisounds);
		}

		CSnd::Close();
		// CSnd::LoadSounds("sounds.rdr", "HUDUI");
		// CSnd::LoadSounds("sounds.rdr", "SMUS");
		CSnd::UIOpen();
	}

	return true;
}

void CMenuState::Tick(f32 dT)
{
	m_menu->Tick(dT);
	
	// The world has to exist in order for nodes to be rendered
	if (zdb::CWorld::m_world != NULL)
	{
		if (zdb::CWorld::m_world->m_camera != NULL)
		{
			zdb::CWorld::m_world->m_camera->Update(zdb::tag_ZCAM_TYPE::ZCAM_NORMAL);
		}
	}

	SDL_Event e;
	SDL_PollEvent(&e);
	zVid_ClearColor(0.0f, 0.0f, 0.0f);
	zVid_Swap(false);
}