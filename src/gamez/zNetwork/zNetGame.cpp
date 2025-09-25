#include "znet.h"

#include "gamez/zReader/zrdr_local.h"
#include "gamez/zUI/zui.h"

bool CZNetGame::Initialize()
{
    CRdrEditor rdr;
    
    m_pTimerValve = NULL;
    m_pTimerAnim = NULL;
    m_mp_major_game_state = MP_MAJOR_GAME_STATE::MP_MAJOR_UNKNOWN;
    m_mp_minor_game_state = MP_MINOR_GAME_STATE::MP_MINOR_UNKNOWN;
    m_my_major_game_state = MP_MAJOR_GAME_STATE::MP_MAJOR_UNKNOWN;
    m_mpe3_round_count = 0;
    m_net_ready_count = 0;
    m_countdown = 0;
    m_net_team = 0;
    m_last_penalty = 0;
    m_gate_clock = 0;
    m_game_object_index = -1;
    m_net_start_flag = MP_NET_READY::NET_READY_UNKNOWN;
    m_bomb_clock_started = false;
    m_persona_created = false;
    m_bomb_created = false;
    m_ready_to_play = false;
    m_ten_second_launch = false;
    m_use_bomb_pos = false;
    m_ghost_ok = false;
    m_pos_smooth = 50.0f;
    m_bdown = false;

    m_uiv_SealList = theUIVarManager.Add("BSEALLISTVAR", NULL, UIVL_PERMANENT);
    m_uiv_TerrList = theUIVarManager.Add("BTERRLISTVAR", NULL, UIVL_PERMANENT);
    m_uiv_NoneList = theUIVarManager.Add("BNONELISTVAR", NULL, UIVL_PERMANENT);
    m_uiv_ChatList = theUIVarManager.Add("BCHATLISTVAR", NULL, UIVL_PERMANENT);

    m_uiv_ChatList->Set(&rdr);

    m_pMajorGameStateValve = CValve::Create("mp_major_game_state", 0, VALVE_TYPE::VTYPE_PERM);

    m_pJoinGameValve = CValve::Create("mp_join_game", VALVE_TYPE::VTYPE_PERM);
    m_pPlayerTeamValve = CValve::Create("player_team", VALVE_TYPE::VTYPE_PERM);
    m_pLateJoinerValve = CValve::Create("late_joiner", VALVE_TYPE::VTYPE_PERM);
    m_was_late_joiner = false;
    m_p45SecondClockValve = CValve::Create("mp_45_sec_clock", VALVE_TYPE::VTYPE_PERM);
    m_pPenaltyValve = CValve::Create("mp_penalty", VALVE_TYPE::VTYPE_PERM);
    m_pPlayerJoinCountValve = CValve::Create("player_join_count", VALVE_TYPE::VTYPE_PERM);
    m_pPlayerReadyCountValve = CValve::Create("player_ready_count", VALVE_TYPE::VTYPE_PERM);
    m_pMPPersistentValve = CValve::Create("mp_persistent", VALVE_TYPE::VTYPE_PERM);
    m_pBombStatusValve = CValve::Create("mp_bomb", VALVE_TYPE::VTYPE_PERM);
    m_pGameOverValve = CValve::Create("mp_game_over", 0, VALVE_TYPE::VTYPE_PERM);
    m_pBombOwnerValve = CValve::Create("bomb_owner", VALVE_TYPE::VTYPE_PERM);
    m_pRoundCountValve = CValve::Create("round_count", 0, VALVE_TYPE::VTYPE_PERM);

    return true;
}

void CZNetGame::MasterStartGame()
{

}