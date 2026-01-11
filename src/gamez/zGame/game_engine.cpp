#include "zgame.h"

#include <SDL3/SDL_time.h>

#include "gamez/zFTS/zfts.h"
#include "gamez/zIMGUI/zimgui.h"

#include "gamez/zNetwork/znet.h"

#include "gamez/zNode/node_world.h"
#include "gamez/zNode/node_assetlib.h"

#include "gamez/zVideo/zvid.h"

char* GetDatabase();

bool CGame::StartEngine()
{
	// zTaskScheduler.Clear();
	// zdb::CRipple::Init();
	// zdb::CTextureFX::Init();
	CValve::Init();
	zVid_Init(ZVID_MODE_NTSC);
	zgl_init();
	zgl_set_context(theWindow->GetWindow(), theWindow->GetRenderer(), theWindow->GetContext());
	zdb::CTexture::Init();
	C2D::Init();
	zdb::CWorld::Init();
	zSysReset();
	zVid_Open();
	C2D::Open();
	CInput::Init();
	CZIMGUI::Init();
	// zRndrInit();
	// CSndInstance::InitInstancePool(48);
	CInput::CreatePad(0);
	CInput::CreatePad(1);
	// zMath_Init();
	// CPipe::Init();
	// CGameStateChangeCmd::CreatePool(16);
	// CSealAnim* sealAnim = new CSealAnim();
	// CZSealBody::m_sealanim = sealAnim;
	return true;
}

bool COurGame::StartEngine()
{
	if (!CGame::StartEngine())
		return false;

	char img_dir[256];
	sprintf_s(img_dir, 256, "%s/LOADING.RAW", gamez_GameRunPath);

	// Display loading screen
	CVideo::RestoreImage(img_dir, true);

	// After the image is loaded into the back buffer,
	// bring it to the main one
	zVid_Swap(true);

	zdb::CTexManager::m_texmanager->doAddBuffer("default", 0.0f, 0.0f);

	theMission.Init();

	SDL_Time ticks;
	srand(SDL_GetCurrentTime(&ticks));

	char* db = GetDatabase();

	if (db)
	{
		size_t dblen = strlen(db);

		if (dblen != 0)
			theMission.PreOpen(db);
	}

	zdb::CAssetLib* fontLib = zdb::CAssetMgr::GetLoadedLibRef("common/assetlib/font");
	fontLib->m_autoload = false;

	theLobby.Init();

	return true;
}