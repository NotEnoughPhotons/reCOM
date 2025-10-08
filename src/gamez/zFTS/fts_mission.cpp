#include "gamez/zAnim/zanim.h"
#include "gamez/zBone/zbone.h"
#include "gamez/zCharacter/zchar.h"
#include "gamez/zFTS/zfts.h"
#include "gamez/zNetwork/znet.h"
#include "gamez/zNode/node_world.h"
#include "gamez/zSeal/zseal.h"
#include "gamez/zSystem/zsys.h"
#include "gamez/zVehicle/zvehicle.h"
#include "gamez/zVideo/zvid.h"

CMission theMission;

void UpdateMissionArchive(const char* type, const char* path)
{
	std::string pathstr;
	pathstr.append(gamez_GameRunPath);

	if (type)
	{
		pathstr.assign("run/", pathstr.length());

		if (strncmp(type, "mp", 2) == 0)
		{
			pathstr.append("mp");
		}
		else
		{
			if (strncmp(type, "m", 1) == 0)
			{
				pathstr.append("sp/");
			}
		}
	}

	if (path)
	{
		if (strlen(path) != 0)
		{
			pathstr.assign(gamez_GameRunPath, pathstr.length());
			pathstr.assign("D:/run/", pathstr.length());

			if (strncmp(path, "mp", 2) == 0)
			{
				pathstr.append("mp/", strlen("mp/"));
			}
			else
			{
				if (strncmp(path, "m", 1) == 0)
				{
					pathstr.append("sp/", strlen("sp/"));
				}
			}

			pathstr.append(path, strlen(path));
			CRdrArchive::AddArchive("readerm.zar", pathstr.c_str());
			CRdrArchive::OpenAll();
		}
	}
}

s32 FilterMissionFolder(const char* prefix, const char* infix, const char* postfix, char* output)
{
	s32 result = 0;

	if (!infix)
	{
		infix = "/";
	}

	// Multiplayer map?
	if (strncmp(prefix, "mp", 2) == 0)
	{
		result = sprintf_s(output, 128, "%s/mp/%s", infix, postfix);
	}
	else
	{
		// Singleplayer map?
		if (strncmp(prefix, "m", 1) == 0)
		{
			result = sprintf_s(output, 128, "%s/sp/%s", infix, postfix);
		}
		else
		{
			result = sprintf_s(output, 128, "%s/%s", infix, postfix);
		}
	}

	return result;
}

bool CMission::Init()
{
	char path_buf[256];
	
	// TODO:
	// Implement all these
	ZAnim.Open();
	CSealCtrlAi::RegisterCommands();
	// CValve::RegisterCommands();
	// zdb::CCamera::RegisterAnimCommands();
	// CAppCamera::RegisterAnimCommands();
	// CftsPlayer::RegisterAnimCommands();
	// zdb::CWind::RegisterAnimCommands();

	ZAnim.AddCmd("HUD_ON", HUDCmdParseOn, NULL, HUDCmdTickControlSwitch, NULL);
	ZAnim.AddCmd("HUD_OFF", HUDCmdParseOff, NULL, HUDCmdTickControlSwitch, NULL);
	ZAnim.AddCmd("HUD_LETTERBOX_ON", HUDLetterBoxCmdParseOn, NULL, HUDLetterBoxCmdTick, NULL);
	ZAnim.AddCmd("HUD_LETTERBOX_OFF", HUDLetterBoxCmdParseOff, NULL, HUDLetterBoxCmdTick, NULL);

	m_chars_loaded = false;

	sprintf_s(path_buf, 256, "%s/ui", gamez_GameRunPath);
	
	CRdrArchive::AddArchive("readerc.zar", gamez_GameRunPath);
	CRdrArchive::AddArchive("readerc.zar", path_buf);
	CRdrArchive::OpenAll();

	zBoneInit();
	zBoneOpen();
	zBoneLoadAll();
	zBoneClose();

	CZNetGame* netgame = new CZNetGame();

	netgame->m_clock.m_max_game_time = 0;
	netgame->m_clock.m_game_time = 0;
	netgame->m_clock.m_time_in_round = 0;
	netgame->m_clock.m_initial_read = 0;
	netgame->m_clock.m_total_time = 0;
	netgame->m_clock.m_initial_read_set = false;
	netgame->m_clock.m_clock_running = false;
	netgame->Initialize();

	m_pNetGame = netgame;

	CRdrFile* hud_rdr = zrdr_read("hud.rdr");

	if (hud_rdr)
	{
		zrdr_findPNT2D(hud_rdr, "static_radio", &m_pNetGame->m_static_radio_loc);
		zrdr_findPNT2D(hud_rdr, "dynamic_radio", &m_pNetGame->m_dynamic_radio_loc);
	}

	zrdr_free(hud_rdr);

	CValve::Open("global_valves.rdr", VALVE_TYPE::VTYPE_PERM);
	m_valve_complete = CValve::Create("mission_complete", VALVE_TYPE::VTYPE_PERM);
	m_valve_failure = CValve::Create("mission_failure", VALVE_TYPE::VTYPE_PERM);
	m_valve_abort = CValve::Create("mission_abort", VALVE_TYPE::VTYPE_PERM);
	m_valve_timeout = CValve::Create("mission_timeout", VALVE_TYPE::VTYPE_PERM);
	m_valve_nofade = CValve::Create("mission_nofade", VALVE_TYPE::VTYPE_PERM);

	m_ordersRdr = zrdr_read("orders.rdr");
	m_materialsRdr = zrdr_read("materials.rdr");
	m_decalsRdr = zrdr_read("decals.rdr");
	m_fontsRdr = zrdr_read("fonts.rdr");
	m_messagesRdr = zrdr_read("messages.rdr");
	m_subtitlesRdr = zrdr_read("subtitles.rdr");
	m_smallmessagesRdr = zrdr_read("small_messages.rdr");

	// Particle::Init();
	SealInitCharacterDynamics();

	m_vehicleRdr = new CVehicleRdr();

	// AnimSet::Load();
	CCharacterType::Open("character.rdr");
	// CCtrlrConfigs::Init()
	
	// ZuiInit();
	hudInit();

	// TODO: Implement CTurret
	//CTurret::Init("ai_turrets.rdr", "data/common");
	CRdrArchive::RemoveArchive("readerc.zar", gamez_GameRunPath);

	return true;
}

void CMission::PreOpen(const char* db)
{
	if (db && *db != '\0')
	{
		UpdateMissionArchive(m_database, db);
		m_database = m_db_string;
		strcpy_s(m_db_string, db);
	}
	else
	{
		m_database = NULL;
		return;
	}

	_zrdr* mission = zrdr_read("mission.rdr");
	char* loadingscreenassets = zrdr_findstring(mission, "LoadingScreenAssets");

	if (!loadingscreenassets)
	{
		m_strLoadingScrLib = "";
	}
	else
	{
		size_t len = strlen(loadingscreenassets);
		m_strLoadingScrLib.assign(&loadingscreenassets, len);
	}

	_zrdr* uivars = zrdr_findtag(mission, "UiVars");
	u32 idx = 0;
}

void CMission::Read(_zrdr* reader)
{
	m_ai_params.weather_factor = 0.0f;
	m_ai_params.fireteam_command = FT_COMMAND::CMD_FOLLOW;
	m_ai_params.recycle_time = 60.0f;
	m_ai_params.recycle_range = 10000.0f;
	m_ai_params.respawn_time = 5.0f;
	m_ai_params.respawn_fade = 0.75f;

	auto aiParams = zrdr_findtag(reader, "ai_params");
	if (aiParams != NULL)
	{
		zrdr_findreal(aiParams, "weather_factor", &m_ai_params.weather_factor, 1);
		zrdr_findreal(aiParams, "respawn_time", &m_ai_params.respawn_time, 1);
		zrdr_findreal(aiParams, "respawn_fade", &m_ai_params.respawn_fade, 1);
		zrdr_findreal(aiParams, "recycle_time", &m_ai_params.recycle_time, 1);
		bool success = zrdr_findreal(aiParams, "recycle_range", &m_ai_params.recycle_range, 1);

		if (success)
		{
			m_ai_params.recycle_range *= m_ai_params.recycle_range;
		}
	}
}

void CMission::OnMissionStart()
{
	VALVE_STATE valveMissionState;

	m_state = MISSION_RUNNING;
	m_delay = -1.0f;
	appCamera->ResetDeathCam();
	m_DelayEndMission = false;
	m_FadingToEndMission = false;

	valveMissionState = VALVE_STATE::VSTATE_DORMANT;

	if (m_valve_complete)
	{
		valveMissionState = VALVE_STATE::VSTATE_PERMANENT;
		m_valve_complete->m_value = 0;
		m_valve_complete->MakeCallbacks(valveMissionState);
	}

	if (m_valve_failure)
	{
		valveMissionState = VALVE_STATE::VSTATE_PERMANENT;
		m_valve_failure->m_value = 0;
		m_valve_failure->MakeCallbacks(valveMissionState);
	}

	if (m_valve_abort)
	{
		valveMissionState = VALVE_STATE::VSTATE_PERMANENT;
		m_valve_abort->m_value = 0;
		m_valve_abort->MakeCallbacks(valveMissionState);
	}

	if (m_valve_timeout)
	{
		valveMissionState = VALVE_STATE::VSTATE_PERMANENT;
		m_valve_timeout->m_value = 0;
		m_valve_timeout->MakeCallbacks(valveMissionState);
	}

	if (m_valve_nofade)
	{
		valveMissionState = VALVE_STATE::VSTATE_PERMANENT;
		m_valve_nofade->m_value = 0;
		m_valve_nofade->MakeCallbacks(valveMissionState);
	}

	zdb::CWorld::m_world->m_diIntDelayCnt = 0;

	CInput::Flush();

	CZAnimSet* common = NULL; // ZAnim.GetAnimSet("common");
	CZAnimSet* mission = NULL;

	if (common)
		common->Start();

	if (mission)
		mission->Start();

	// TODO: Animation init loop

	if (!theNetwork.m_bNetwork)
		CSnd::Open();
	else
		CSnd::NetOpen();

	CSnd::m_listenerIsValid = true;
	CSndJukebox::Start();

	if (theNetwork.m_bNetwork)
	{
		// CPickup::Open(CEntity::m_list);
		theHUD->ReInit_Uninit();
		theHUD->ReInit_Init();

		if (m_pNetGame)
		{
			if (theNetwork.m_bSessionMaster)
				m_pNetGame->m_mp_major_game_state = MP_MAJOR_GAME_STATE::MP_MAJOR_INPLAY;

			m_pNetGame->MasterStartGame();
		}
	}
}

void CMission::OnMissionComplete(MISSION_STATE state)
{
	if (state != m_state)
	{
		m_state = state;
	}

	if (!theNetwork.m_bNetwork)
	{
		if (m_state == MISSION_ABORTED || !ftsPlayer->m_isAlive)
		{
			if (m_valve_nofade)
			{
				m_DelayEndMission = false;

				// _OnMissionComplete();
				CVideo::Clear(0, 0, 0, 0);
				// CFlashFXVector::Reset();
				theMission.m_DelayEndMission = false;
				m_FadingToEndMission = false;
				appCamera->ResetDeathCam();
				// theGame.Push(&m_exitState, 0);
			}
			else if (!m_FadingToEndMission)
			{
				m_FadingToEndMission = true;
				theMission.m_FadingToEndMission = true;
				m_fade_counter = 0;
				m_next_fade_time = 1.0f;

				// zTaskScheduler.AddTask("MissionFader", MissionFadeToEnd, fadeToEndMissionTick, 0.98f, this);
			}
			else
			{
				if (m_next_fade_time > 1.0f)
				{
					m_next_fade_time = 1.0f;
				}

				if (m_fade_counter > 0)
				{
					m_fade_counter = 0;
				}
			}
		}
	}
	else
	{
		if (m_pNetGame)
		{
			// CZBombState::UninitBomb;

			if (state == MISSION_ABORTED)
			{
				if (m_delay <= 1.0f)
				{
					m_delay = m_timer + 3.0f;
				}
				else if (m_delay < m_timer)
				{
					
				}
			}
		}
	}
}

void CMission::Tick(f32 dT)
{
	m_timer += dT;

	if (m_wait_before_falling > 0.0f)
	{
		m_wait_before_falling -= dT;
	}

	m_winlosstimer += m_winlossinc * dT;

	if (!theNetwork.m_bNetwork)
	{
		if (m_pNetGame == NULL)
		{
			// TickObjectives(dT);
		}
	}
	else
	{
		// CNetClock::Tick();
	}

	if (!m_ignore_objectives)
	{
		if (!m_valve_timeout->m_value)
		{
			OnMissionComplete(MISSION_TIMEOUT);
		}

		if (!m_valve_abort->m_value)
		{
			OnMissionComplete(MISSION_ABORTED);
		}

		if (!m_valve_failure->m_value)
		{
			OnMissionComplete(MISSION_FAILURE);
		}

		if (!m_valve_complete->m_value)
		{
			OnMissionComplete(MISSION_SUCCESS);
		}
	}
	else if (m_valve_abort->m_value)
	{
		OnMissionComplete(MISSION_ABORTED);
	}

	// spawners.Tick(dT);
	TickInFirefight(dT);
}

void CMission::TickInFirefight(f32 dT)
{
	// The "awareness counter" keeps track of how many entities -
	// saw the player. Enemies update the aware counter from -
	// animation events and their state machine.

	// This also controls the jukebox, where it will play combat -
	// music if at least ONE enemy entity saw you.
	
	u32 aware_counter = 0;
	
	if (ftsPlayer)
	{
		aware_counter = ftsPlayer->m_aware_counter;
	}

	// Is the player seen by anyone else?
	if (aware_counter < 1)
	{
		// Decrease the fight timer
		if (m_fighttimer > 0.0f)
		{
			m_fighttimer =- dT;
		}

		// We are no longer fighting enemies.
		// Go back to using stealth music.
		if (m_fighttimer <= 0.0f)
		{
			m_fighttimer = 0.0f;
			CSndJukebox::IsInFight(false);
		}
	}
	else
	{
		// Player is currently in a fight
		// Let the jukebox know!
		if (m_fighttimer <= 0.0f)
		{
			CSndJukebox::IsInFight(true);
		}

		m_fighttimer = 4.0f;
	}
}
