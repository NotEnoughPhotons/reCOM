#include "zcam.h"

#include "gamez/zCharacter/zchar.h"
#include "gamez/zEntity/zentity.h"
#include "gamez/zFTS/zfts.h"
#include "gamez/zSeal/zseal.h"
#include "gamez/zSystem/zsys.h"

CAppCamera* appCamera = NULL;

bool camera_tether_intersects_something;
f32 save_short_tether_length;
f32 DeltaRot;
f32 DeltaAim;

f32 peekCur = 0.0f;

void GetFirstPersonCameraPos(CAppCamera* camera)
{
	
}

void FirstToThirdPersonTransition(CAppCamera* camera)
{
	
}

void ThirdToFirstPersonTransition(CAppCamera* camera)
{
	
}

CAppCamera::CAppCamera(zdb::CWorld* world, zdb::CCamera* camera)
{
	m_cameraAim.x = 0.0f;
	m_cameraAim.y = 20.0f;
	m_cameraAim.z = 0.0f;

	m_cameraTie.x = 0.0f;
	m_cameraTie.y = 20.0f;
	m_cameraTie.z = 0.0f;

	m_camGoalPos.x = 40.0f;
	m_camGoalPos.y = 20.0f;
	m_camGoalPos.z = 0.0f;

	m_short_tether_timer = -1.0f;

	m_padid = 0;

	m_camera = camera;
	m_camera_effects = NULL;
	m_entity = NULL;

	m_z_nominal = 500.0f;
	m_camera_mode = PLAYER_CAM_STATE::cam_mode_tether;
	m_camera_last_mode = PLAYER_CAM_STATE::cam_mode_tether;

	m_death_state_timer = 0;

	f32 goalX = m_camGoalPos.x;
	f32 goalY = m_camGoalPos.y;
	f32 goalZ = m_camGoalPos.z;
	f32 goalSqr = (goalZ * goalZ) + (goalX * goalX) + (goalY * goalY);
	m_goal_t_len = goalSqr;

	CZSealBody* player = ftsGetPlayer();

	if (player)
	{
		bool isPlayer = false;

		if (player->m_control != NULL)
		{
			isPlayer = ((CSealCtrl*)player->m_control)->IsPlayer();
		}

		if (isPlayer)
		{
			m_padid = ((CSealCtrl*)player->m_control)->m_padid;
		}
	}

	m_entity = player;

	if (!m_entity)
	{
		m_camera_effects = NULL;
		m_skel_root = NULL;
	}
	else
	{
		zdb::CNode* playerNode = m_entity->GetNode();
		zdb::CNode* cameraEffects = playerNode->FindChild("camera_effects", true);
		m_camera_effects = cameraEffects;
		m_skel_root = m_entity->FindPart("skel_root");
	}

	m_wiggle_amp = theCharacterDynamics.m_cam_wiggle.amplitude;
	m_wiggle_duration = theCharacterDynamics.m_cam_wiggle.duration;
	m_wiggle_inv_duration = 1.0f / m_wiggle_duration;
	m_wiggle_rate = theCharacterDynamics.m_cam_wiggle.rate;
	m_wiggle_inv_rate = 1.0f / m_wiggle_rate;

	if (m_wiggle_echo)
	{
		m_wiggle_scalar = 1.0f;
	}
	else if (m_wiggle_scalar < 1.0f)
	{
		m_wiggle_scalar = 1.0f;
	}

	m_wiggle_cur_time = 0.0f;
	m_wiggle_echo = false;
}

CAppCamera::~CAppCamera()
{
	m_skel_root = NULL;
	m_entity = NULL;
	m_camera_effects = NULL;
	m_camera = NULL;
}

_zanim_cmd_hdr* CAppCamera::CmdParse3rdPersonTest(_zrdr* reader)
{
	_zanim_cmd_hdr* cmd = NULL;

	if (!reader)
	{
		cmd = NULL;
	}
	else
	{
		cmd = static_cast<_zanim_cmd_hdr*>(zcalloc(1, sizeof(_zanim_cmd_hdr)));
		cmd->quad_align = false;
		cmd->timeless = false;
	}

	return cmd;
}

bool CAppCamera::CmdTick3rdPersonTest(_zanim_cmd_hdr* header, f32* delta)
{
	return appCamera->m_camera_mode != PLAYER_CAM_STATE::cam_mode_FP;
}

void CAppCamera::RegisterAnimCommands()
{
	ZAnim.AddCmd("CAMERA_3RD_PERSON", CmdParse3rdPersonTest, NULL, CmdTick3rdPersonTest, NULL);
}

void CAppCamera::APFirstPersonCam(CMatrix& matrix)
{
	
}

void CAppCamera::APLookAt(f32 dT, CEntity* entity, CMatrix& matrix)
{
	
}

void CAppCamera::APTeatherCam(CMatrix& transform, float offset)
{
	
}

void CAppCamera::UpdateDeathCamState()
{
	
}


void CAppCamera::FTSTick(f32 dT)
{
	bool isPlayer = false;
	
	if (m_entity)
	{
		if (!m_entity->m_control)
		{
			isPlayer = false;
		}
		else
		{
			isPlayer = ((CSealCtrl*)m_entity->m_control)->IsPlayer();
		}

		if (isPlayer)
		{
			m_padid = ((CSealCtrl*)m_entity->m_control)->m_padid;
		}

		CPad* pad = NULL;
		
		if (m_padid < 2)
		{
			pad = CInput::m_pads[m_padid];
		}

		if (!pad)
		{
			pad = CInput::m_pads[0];
		}

		bool isButtonsDown = pad->GetTwoButtons(PAD_BUTTON::PAD_SELECT, PAD_BUTTON::PAD_DOWN);

		if (pad && isButtonsDown)
		{
			// m_ctrl_view = m_ctrl_view % (CAMVIEW)theCharacterDynamics.m_cam_params.size();
		}

		if (m_ctrl_view != m_save_view)
		{
			m_camGoalPos = theCharacterDynamics.m_cam_params.front().m_cam_offset;
			m_cameraAim = theCharacterDynamics.m_cam_params.front().m_cam_aimpoint;

			f32 x = m_camGoalPos.x;
			f32 y = m_camGoalPos.y;
			f32 z = m_camGoalPos.z;

			m_goal_t_len = sqrtf(z * z + y * y + x * x);

			if (m_entity)
			{
				CMatrix lookat = CMatrix::identity;
				m_entity->m_node->m_matrix.Transform(&m_camGoalPos, 1);
				m_entity->m_node->m_matrix.Transform(&m_cameraAim, 1);
				LookAt(&m_camGoalPos, &m_cameraAim, lookat);
				m_camera->SetMatrix(&lookat);
			}

			m_save_view = m_ctrl_view;

			if (m_ctrl_view == CAMVIEW::cam_view_first)
			{
				m_camera_mode = PLAYER_CAM_STATE::cam_mode_FP;
			}
			else
			{
				m_camera_mode = PLAYER_CAM_STATE::cam_mode_tether;
			}
		}
	}
}


void CAppCamera::LookAt(CPnt3D* center, CPnt3D* eye, CMatrix& mat)
{

}

void CAppCamera::SetZoom(f32 zoom)
{
	m_camera->m_Zmin = zoom;
	m_camera->m_Zmax = zoom;
	m_camera->m_RangeScale = 1.0f / zoom;
}

void CAppCamera::ResetDeathCam()
{
	m_death_state_timer = 0;
	m_allow_player_death_cam_control = true;
	save_short_tether_length = 10000.0f;
	DeltaRot = 0.0f;
	DeltaAim = 0.0f;
}

void CAppCamera::Tick(f32 dT)
{
	if (!zdb::CCamera::m_dynamics_controlled)
	{
		return;
	}

	CMatrix firstperson = CMatrix::identity;
	FTSTick(dT);
	
	if (m_camera_mode != PLAYER_CAM_STATE::cam_mode_net)
	{
		if (m_camera_mode == PLAYER_CAM_STATE::cam_mode_apLook)
		{
			// APLookAt();
		}
		else if (m_camera_mode == PLAYER_CAM_STATE::cam_mode_FP)
		{
			if (m_camera_last_mode == PLAYER_CAM_STATE::cam_mode_tether)
			{
				ThirdToFirstPersonTransition(this);
			}

			APFirstPersonCam(firstperson);
		}
		else
		{
			if (m_camera_mode == PLAYER_CAM_STATE::cam_mode_tether)
			{
				if (m_camera_last_mode == PLAYER_CAM_STATE::cam_mode_FP)
				{
					FirstToThirdPersonTransition(this);
				}
				
				APTeatherCam(firstperson, dT);
			}
			
			APTeatherCam(firstperson, dT);
		}
	}

	m_camera_last_mode = m_camera_mode;

	if (m_camera_effects)
	{
		
	}
}

void CAppCamera::TickCameraWiggle(f32 dT, zdb::CCamera* camera)
{
	f32 wiggle = 0.0f;
	f32 wiggle_cur_time = m_wiggle_cur_time;

	if (m_wiggle_duration < wiggle_cur_time)
	{
		camera->m_screen.right = 0;
		camera->m_screen.bottom = 0;
		m_wiggle_echo = false;
	}
	else
	{
		f32 wiggle_inv_duration = wiggle_cur_time * m_wiggle_inv_duration;
		wiggle_cur_time *= 2.0f * PI * m_wiggle_inv_rate;

		if (wiggle_inv_duration < 0.0f)
		{
			wiggle_inv_duration = 0.0f;
		}

		wiggle = 1.0f;

		if (wiggle_inv_duration <= 1.0f)
		{
			wiggle = wiggle_inv_duration;
		}

		wiggle = (1.0f - wiggle) * m_wiggle_amp * m_wiggle_scalar;
		wiggle_inv_duration = sinf(wiggle_cur_time);
		camera->m_screen.right = wiggle * wiggle_cur_time + camera->m_screen.right;
		camera->m_screen.bottom = wiggle * 0.5 * wiggle_inv_duration + camera->m_screen.bottom;
		m_wiggle_cur_time += dT;
	}
}

void CAppCamera::WiggleEcho(bool echo)
{
	m_wiggle_echo = echo;

	if (echo)
	{
		m_wiggle_cur_time = 0.0f;
	}
}

void CAppCamera::WiggleScalar(f32 wiggle)
{
	if (!m_wiggle_echo)
	{
		m_wiggle_scalar = wiggle;
	}
	else if (m_wiggle_scalar < wiggle)
	{
		m_wiggle_scalar = wiggle;
	}
}