#include "znode.h"
#include "node_saveload.h"

#include "gamez/zArchive/zar.h"
#include "gamez/zFTS/zfts.h"

#include "gamez/zNode/node_world.h"
#include "gamez/zNode/node_assetlib.h"

#include "gamez/zGrid/zgrid.h"
#include "gamez/zSave/zsave.h"
#include "gamez/zPhysics/zphysics.h"
#include "gamez/zSystem/zsys.h"

namespace zdb
{
	bool CNode::ReadDataBegin(CSaveLoad& sload)
	{
		zar::CKey* openkey = sload.m_zfile.GetOpenKey();
		SetName(openkey->GetName());
		zar::CKey* visualskey = sload.m_zfile.OpenKey("visuals");

		// If the node has visuals
		if (visualskey)
		{
			ReserveVisuals(visualskey->GetSize());
			auto it = visualskey->begin();

			while (it != visualskey->end())
			{
				zar::CKey* key = sload.m_zfile.OpenKey(*it);
				
				if (key)
				{
					CVisual* vis = CVisual::Create(sload.m_zfile);

					if (vis)
					{
						AddVisual(vis);
					}

					sload.m_zfile.CloseKey(key);
				}
			}

			sload.m_zfile.CloseKey(visualskey);
		}

		zar::CKey* dikey = sload.m_zfile.OpenKey("di");

		if (dikey)
		{
			ReserveDI(dikey->GetSize());

			auto it = dikey->begin();

			while (it != dikey->end())
			{
				zar::CKey* key = sload.m_zfile.OpenKey(*it);

				if (key)
				{
					CDI* di = CDI::Create(sload);

					if (di)
					{
						AddDI(di);
					}

					sload.m_zfile.CloseKey(key);
				}
			}

			sload.m_zfile.CloseKey(dikey);
		}

		return false;
	}
	
	CSaveLoad::CSaveLoad()
	{
		m_world = NULL;
		m_zfile = zar::CZAR();
		m_zed_filename[0] = '\0';
		m_version = 0;
	}

	bool CSaveLoad::LoadAssetLib(CWorld* world, CAssetLib* library, u32 type)
	{
		return LoadAssetLib_PS2(world, library, type);
	}

	bool CSaveLoad::LoadAssetLib_PS2(CWorld* world, CAssetLib* library, u32 type)
	{
		bool success = true;

		m_world = world;

		if ((type & TYPE_TEXTURES) != 0)
		{
			std::string temp(library->RootName());
			temp.assign(temp.substr(0, 4));

			sprintf_s(m_zed_filename, 256, "%s%s/%s%s.zed", gamez_GameRunPath, library->m_name, temp.c_str(), "_txr");

			if (!m_zfile.Open(m_zed_filename, CWorld::GetVersion(), 1, 16))
			{
				m_zfile.Close();
				success = false;
			}
			else
			{
				success = LoadTextures_PS2(library);
				m_zfile.Close();
			}
		}

		if ((type & TYPE_PALETTES) != 0)
		{
			std::string temp(library->RootName());
			temp.assign(temp.substr(0, 4));

			sprintf_s(m_zed_filename, 256, "%s%s/%s%s.zed", gamez_GameRunPath, library->m_name, temp.c_str(), "_pal");

			if (!m_zfile.Open(m_zed_filename, CWorld::GetVersion(), 1, 16))
			{
				m_zfile.Close();
				success = false;
			}
			else
			{
				success = LoadPalettes_PS2(library);
				m_zfile.Close();
			}
		}

		if ((type & TYPE_MODELS) != 0)
		{
			std::string temp(library->RootName());
			temp.assign(temp.substr(0, 4));

			sprintf_s(m_zed_filename, 256, "%s%s/%s%s.zed", gamez_GameRunPath, library->m_name, temp.c_str(), "_mdl");

			if (!m_zfile.Open(m_zed_filename, CWorld::GetVersion(), 1, 16))
			{
				m_zfile.Close();
				success = false;
			}
			else
			{
				if (auto key = m_zfile.OpenKey("models"))
				{
					auto it = key->begin();
					while (it != key->end())
					{
						if (auto model_key = m_zfile.OpenKey(*it))
						{
							CModel* model = CModel::Create(*this, NULL);
							library->AddModel(model);
							m_zfile.CloseKey(model_key);
						}

						++it;
					}

					m_zfile.CloseKey(key);
				}

				m_zfile.Close();
			}
		}

		return success;
	}

	bool CSaveLoad::LoadTextures_PS2(CAssetLib* library)
	{
		CTexture* texture = NULL;
		s32 renderphase;

		// TODO: fix renderphase enum
		if (m_zfile.Fetch("renderphase", &renderphase))
		{

		}

		zar::CKey* key = m_zfile.OpenKey("textures");

		if (key)
		{
			auto it = key->begin();

			while (it != key->end())
			{
				zar::CKey* texturekey = m_zfile.OpenKey(*it);

				if (texturekey)
				{
					char* name = texturekey->GetName();
					CTexHandle* handle = CAssetMgr::m_assets.GetTexHandle(name);

					if (!handle && !library->AddTexture(name))
					{
						texture = new CTexture(name);
						texture->Read(*this);
						library->AddTexture(texture);
					}

					m_zfile.CloseKey(texturekey);
				}

				++it;
			}

			m_zfile.CloseKey(key);
		}

		library->m_texture_buffer = m_zfile.ReleaseDataBuffer();
		return true;
	}

	CWorld* CSaveLoad::Load(const char* name)
	{
		f32 metersperunit = 0.0f;
		CPnt3D shadowvector = CPnt3D::zero;
		CPnt3D starlightscope = CPnt3D::zero;
		bool gridterrain = false;
		bool nightmission = false;
		char* defaultmaterial = NULL;
		
		zdb::CWorld* world = NULL;
		
		FilterMissionFolder(name, gamez_GameRunPath, name, m_zed_filename);
		strcat_s(m_zed_filename, "/");
		strcat_s(m_zed_filename, name);
		strcat_s(m_zed_filename, ".zed");

		u32 version = zdb::CWorld::GetVersion();

		if (!m_zfile.Open(m_zed_filename, version, 1, 16))
		{
			m_zfile.Close();
			world = NULL;
		}

		world = new CWorld(name);

		m_world = world;
		m_world->Initalize();

		metersperunit = 1.0f;
		shadowvector = CPnt3D::zero;
		gridterrain = false;
		defaultmaterial = NULL;

		m_zfile.Fetch("NightMission", &nightmission);
		m_world->m_night_mission = nightmission != false;

		if (m_zfile.Fetch("LensFX_NVG", &shadowvector, sizeof(CPnt3D)))
		{
			m_world->m_shadow_vector = shadowvector;
		}

		if (m_zfile.Fetch("LensFX_StarlightScope", &starlightscope, sizeof(CPnt3D)))
		{
			m_world->m_lensfx_starlightscope = starlightscope;
		}

		m_zfile.Fetch("DefaultMaterial", &defaultmaterial);
		m_zfile.Fetch("MetersPerUnit", &metersperunit);
		m_zfile.Fetch("GriddedTerrain", &gridterrain);
		m_zfile.Fetch("ShadowVector", &shadowvector, sizeof(CPnt3D));
		m_zfile.Fetch("GlobalLighting", &m_world->m_gLight, sizeof(_globalLight));
			
		CWorld::m_scale = 1.0f / metersperunit;
		CWorld::m_invscale = 1.0f / CWorld::m_scale;
			
		shadowvector.Normalize();

		m_world->m_shadow_vector = shadowvector;

		if (defaultmaterial)
		{
			m_world->m_default_soiltype_name = zstrdup(defaultmaterial);
			zfree(defaultmaterial);
			defaultmaterial = NULL;
		}

		if (zar::CKey* key = m_zfile.OpenKey("cameras"))
		{
			m_zfile.Fetch("camera", m_world->m_camera, sizeof(tag_CAMERA_PARAMS));
			CVisual::m_basefog_color = m_world->m_camera->m_fog_color;
			m_world->m_camera->SetHalfHorizontalFOVRadians(0.61008652f);
			m_zfile.CloseKey(key);
		}

		m_world->m_grid->Read(*this);

		zar::CKey* key = m_zfile.OpenKey("assetlibs");
			
		if (key)
		{
			auto it = key->begin();

			while (it != key->end())
			{
				CAssetMgr::GetLoadedLibRef((*it)->GetName());
				++it;
			}

			m_zfile.CloseKey(key);
		}

		key = m_zfile.OpenKey("light_list");

		if (key)
		{
			auto it = key->begin();
			while (it != key->end())
			{
				zar::CKey* light_key = m_zfile.OpenKey(*it);

				if (light_key)
				{
					CNode::Read(*this, m_world);
					m_zfile.CloseKey(light_key);
				}
				
				++it;
			}

			m_zfile.CloseKey(key);
		}
		
		m_world->Read(*this);
			
		m_zfile.Close();

		return world;
	}
}