#include "zcam.h"

#include "gamez/zMath/zmath.h"
#include "gamez/zNode/node_world.h"
#include "gamez/zSeal/zseal.h"

bool fogCheck = true;

namespace zdb
{
	bool CCamera::m_dynamics_controlled = true;
	bool CCamera::m_do_region_test = true;
	
	CCamera::CCamera() : CNode()
	{
		m_position = glm::vec3(0.0f, 0.0f, 0.0f);
		m_target = glm::vec3(0.0f, 0.0f, 0.0f);
		m_direction = glm::vec3(0.0f, 0.0f, 0.0f);
		m_right = glm::vec3(0.0f, 0.0f, 0.0f);
		m_up = glm::vec3(0.0f, 0.0f, 0.0f);
		m_forward = glm::vec3(0.0f, 0.0f, 0.0f);
		m_yaw = 0.0f;
		m_pitch = 0.0f;
	}
	
	void CCamera::RegisterAnimCommands()
	{
		ZAnim.AddCmd("DYNAMICS_RELEASE_CAMERA", CmdParseRelease, NULL, CmdTickControlSwitch, NULL);
		ZAnim.AddCmd("DYNAMICS_ACQUIRE_CAMERA", CmdParseAcquire, NULL, CmdTickControlSwitch, NULL);
		ZAnim.AddCmd("CAMERA_INDOORS", CmdParseIndoorsTest, NULL, CmdTickIndoorsTest, NULL);
		ZAnim.AddCmd("SET_CAMERA_REGION_TEST", CmdParseSetRegions, NULL, CmdTickSetRegions, NULL);
		ZAnim.AddCmd("GET_CAMERA_REGION_TEST", CmdParseGetRegions, NULL, CmdTickGetRegions, NULL);
		ZAnim.AddCmd("CAMERA_PARAMS", CmdParseParams, CmdBeginParams, CmdTickParams, NULL);
	}

	void CCamera::CmdBeginParams(_zanim_cmd_hdr* header)
	{
		CCamera* worldCamera = zdb::CWorld::m_world->m_camera;

		if (worldCamera != NULL)
		{

		}
	}

	_zanim_cmd_hdr* CCamera::CmdParseRelease(_zrdr* reader)
	{
		_zanim_cmd_hdr* cmd = NULL;
		return cmd;
	}

	_zanim_cmd_hdr* CCamera::CmdParseAcquire(_zrdr* reader)
	{
		_zanim_cmd_hdr* cmd = NULL;
		return cmd;
	}

	_zanim_cmd_hdr* CCamera::CmdParseIndoorsTest(_zrdr* reader)
	{
		_zanim_cmd_hdr* cmd = NULL;
		return cmd;
	}

	_zanim_cmd_hdr* CCamera::CmdParseSetRegions(_zrdr* reader)
	{
		_zanim_cmd_hdr* cmd = NULL;
		return cmd;
	}

	_zanim_cmd_hdr* CCamera::CmdParseGetRegions(_zrdr* reader)
	{
		_zanim_cmd_hdr* cmd = NULL;
		return cmd;
	}

	_zanim_cmd_hdr* CCamera::CmdParseParams(_zrdr* reader)
	{
		_zanim_cmd_hdr* cmd = NULL;
		return cmd;
	}

	bool CCamera::CmdTickControlSwitch(_zanim_cmd_hdr* header, f32* dT)
	{
		if (header->timeless)
		{
			// m_dynamics_controlled = false;
		}
		else if (!header->timeless)
		{
			// m_dynamics_controlled = true;
		}

		return true;
	}

	bool CCamera::CmdTickIndoorsTest(_zanim_cmd_hdr* header, f32* dT)
	{
		return true;
	}

	bool CCamera::CmdTickSetRegions(_zanim_cmd_hdr* header, f32* dT)
	{
		return true;
	}

	bool CCamera::CmdTickGetRegions(_zanim_cmd_hdr* header, f32* dT)
	{
		return true;
	}

	bool CCamera::CmdTickParams(_zanim_cmd_hdr* header, f32* dT)
	{
		return true;
	}

	bool CCamera::TestLandmarkFOG(CPnt3D* pos, f32 radius)
	{
		bool canSee = false;

		if (m_fog_enabled)
		{
			canSee = false;

			if (m_directional_fog_enabled)
			{
				if (m_landmark_fog_top <= pos->y - radius)
					canSee = false;
			}

			CPnt3D& camPos = (CPnt3D&)(m_matrix[3][0]);
			CPnt3D out;
			pos->Sub(&camPos, &out);

			f32 dist = sqrtf((camPos.z * camPos.z) + (camPos.y * camPos.y) + (camPos.x * camPos.x));
			if (radius + dist < m_fog_near)
				canSee = false;
		}

		return canSee || (m_fog_enabled & fogCheck) != 0;
	}

	bool CCamera::CanSeeRegion(u32 mask)
	{
		if (!mask)
			mask = 1;

		if (m_do_region_test && (m_SeeRegions & mask) != 0)
			return true;

		return false;
	}

	void CCamera::SetHalfHorizontalFOVRadians(f32 radians)
	{
		m_hfov = radians;
		m_sin.x = sinf(radians);
		m_cos.x = cosf(radians);
		m_tan.x = m_sin.x / m_cos.x;
		m_cot.x = 1.0f / m_tan.x;
		m_scrZ = m_farCorner[0][2] * m_cot.x;
	}

	void CCamera::SetFarClip(f32 farclip)
	{
		m_far_plane = farclip;
		m_farCorner[0][1] = farclip;
	}
}