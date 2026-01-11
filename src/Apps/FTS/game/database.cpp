#include "gamez/zArchive/zar.h"
#include "gamez/zAssetLib/zassetlib.h"
#include "gamez/zCamera/zcam.h"
#include "gamez/zNode/znode.h"
#include "gamez/zNode/node_world.h"
#include "gamez/zNode/node_universe.h"
#include "gamez/zNode/node_saveload.h"
#include "gamez/zNode/node_assetlib.h"
#include "gamez/zRender/zrender.h"
#include "gamez/zSave/zsave.h"
#include "gamez/zVisual/zvis.h"

char database[32];

extern zar::CZAR archive = zar::CZAR();
extern zdb::CWorld* theWorld = NULL;

extern char* GetDatabase()
{
	return database;
}

extern char* SetDatabase(const char* db)
{
#ifdef DEBUG
	SDL_Log("Set database to %s", db);
#endif
	return strcpy(database, db);
}

extern bool LoadWorld(const char* name)
{
#if DEBUG
	SDL_Log("[GameZ] - Loading world %s", name);
#endif

	char strbuf[1024];
	zdb::CNodeUniverse* universe = zdb::NodeUniverse;

	if (!universe)
	{
		universe = new zdb::CNodeUniverse();
		universe->m_locked = false;
	}

	zdb::NodeUniverse = universe;
	zdb::NodeUniverse->m_locked = false;
	zdb::CSaveLoad sload = zdb::CSaveLoad();

	theWorld = sload.Load(name);

	if (!theWorld)
	{
		zdb::NodeUniverse->m_locked = true;
		return false;
	}
	
	appCamera = new CAppCamera(theWorld, theWorld->m_camera);
	char pathbuf[64];
		 
	for (auto i = zdb::CAssetMgr::m_assets.begin(); i != zdb::CAssetMgr::m_assets.end(); ++i)
	{
		zdb::CAssetLib* library = *i;

		if (library->m_autoload)
		{
			sprintf_s(pathbuf, 64, "%s/%s/models.zar", gamez_GameRunPath, library->m_name);
	
			if (archive.Open(pathbuf, -1, 1, 16))
			{
				if (!library->m_models.empty())
				{


					for (u32 j = 0; j != library->m_models.size(); j++)
					{
#if DEBUG
						SDL_Log("[GameZ] - Hooking up visuals for model %s", library->m_models[j]->m_name);
#endif
						hookupVisuals(&archive, library->m_models[j]);
					}

					library->m_model_buffer = archive.ReleaseDataBuffer();
				}
					
				archive.Close(true);
			}
		}
	}                                        

	archive.Close();

	if (archive.m_filename)
		zfree(archive.m_filename);

	archive.m_filename = NULL;

	theWorld->DismemberWorldModel();
	theWorld->GenerateLandmarkList();
	theWorld->InheritRegionMasks(0, 0);
	theWorld->GenerateLightList();

	// TODO: Add lightmap texture code here
	// End texture code

	if (theWorld->m_numNoFarClipNodes == 0)
	{
		if (theWorld->m_camera->m_mid_plane < theWorld->m_camera->m_far_plane)
			theWorld->m_camera->SetFarClip(theWorld->m_camera->m_mid_plane);
	}

	if (zdb::NodeUniverse)
		zdb::NodeUniverse->m_locked = true;

	thePipe.Flush();

	return true;
}

extern bool UnloadTheWorld()
{
	return false;
}

extern bool testLoad()
{
	return false;
}

extern bool testUnload()
{
	return false;
}