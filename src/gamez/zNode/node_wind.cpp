#include "node_wind.h"

namespace zdb
{
	void CWind::Reset()
	{
		m_ZenithGoal = 0.0f;
		m_AzimuthGoal = 0.0f;
		m_SpeedGoal = 0.0f;
		m_TickGoal = false;

		m_Zenith = 0.0f;
		m_Azimuth = 0.0f;
		m_Speed = 8.0f;

		m_AzimuthOffset = 0.0f;
		m_MinAzimuthOffset = -0.7853982f;
		m_MaxAzimuthOffset = 0.7853982f;

		m_ZenithOffset = 0.0f;
		m_MinZenithOffset = -0.7853982f;
		m_MaxZenithOffset = 0.7853982f;

		m_AzimuthOffsetVelocity = 0.2617994f;
		m_ZenithOffsetVelocity = 0.08726646f;

		m_SpeedOffset = 3.0f;
		m_MinSpeedOffset = -7.5f;
		m_MaxSpeedOffset = 9.0f;
		m_SpeedOffsetVelocity = 3.0f;
	}
}