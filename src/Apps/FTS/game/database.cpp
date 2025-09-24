#include "gamez/zArchive/zar.h"
#include "gamez/zAssetLib/zassetlib.h"
#include "gamez/zCamera/zcam.h"
#include "gamez/zNode/znode.h"
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
	return strcpy(database, db);
}

extern bool LoadWorld(const char* name)
{
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
		 
	for (auto it = zdb::CAssetMgr::m_assets.begin(); it != zdb::CAssetMgr::m_assets.end(); ++it)
	{
		zdb::CAssetLib* library = *it;

		if (library->m_autoload)
		{
			sprintf_s(pathbuf, 64, "%s/%s/models.zar", gamez_GameRunPath, library->m_name);
	
			if (archive.Open(pathbuf, -1, 1, 16))
			{
				if (!library->m_models.empty())
				{
					s32 i = 0;

					do
					{
						hookupVisuals(&archive, library->m_models[i]);
						i++;
					}
					while (i < library->m_models.size());

					void* data = archive.ReleaseDataBuffer();
					library->m_model_buffer = data;
				}
					
				archive.Close(true);
			}
		}

		archive.Close();

		if (archive.m_filename)
		{
			zfree(archive.m_filename);
		}

		archive.m_filename = NULL;

		theWorld->DismemberWorldModel();
		theWorld->GenerateLandmarkList();
		// theWorld->InheritRegionMasks(0, 0);
		// theWorld->GenerateLightList();
	}                                        

	if (theWorld->m_numNoFarClipNodes == 0)
	{
		if (theWorld->m_camera->m_mid_plane < theWorld->m_camera->m_far_plane)
		{
			theWorld->m_camera->SetFarClip(theWorld->m_camera->m_mid_plane);
		}
	}

	auto it = zdb::CAssetMgr::m_assets.begin();

	while (it != zdb::CAssetMgr::m_assets.end())
	{
		zdb::CAssetLib* library = *it;

		if (!library->m_autoload)
		{
			++it;
			continue;
		}
		
		sprintf_s(pathbuf, 64, "%s/%s/models.zar", gamez_GameRunPath, library->m_name);
		
		if (archive.Open(strbuf, -1, 1, 16))
		{
			if (!library->m_models.empty())
			{
				s32 i = 0;
				
				do
				{
					hookupVisuals(&archive, library->m_models[i++]);
				}
				while (i < library->m_models.size());
			}

			library->m_model_buffer = archive.ReleaseDataBuffer();
		}
		
		++it;
	}
	
	if (zdb::NodeUniverse)
	{
		zdb::NodeUniverse->m_locked = true;
	}

	thePipe.Flush();
	
	return false;
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