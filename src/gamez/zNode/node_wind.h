#ifndef NODE_WIND_H
#define NODE_WIND_H
#include "znode.h"

namespace zdb
{
	class CWind
	{
	public:
		static void RegisterAnimCommands();
		static void CmdParseWind();

		void Reset();

		f32 m_AzimuthGoal;
		f32 m_ZenithGoal;
		f32 m_SpeedGoal;

		f32 m_AzimuthGoalDelta;
		f32 m_ZenithGoalDelta;
		f32 m_SpeedGoalDelta;

		f32 m_Azimuth;

		CPnt4D m_Wind;

		f32 m_Zenith;
		f32 m_Speed;

		f32 m_AzimuthOffset;
		f32 m_MinAzimuthOffset;
		f32 m_MaxAzimuthOffset;

		f32 m_ZenithOffset;
		f32 m_MinZenithOffset;
		f32 m_MaxZenithOffset;

		f32 m_AzimuthOffsetVelocity;
		f32 m_ZenithOffsetVelocity;

		f32 m_SpeedOffset;
		f32 m_MinSpeedOffset;
		f32 m_MaxSpeedOffset;

		f32 m_SpeedOffsetVelocity;

		bool m_TickAzimuth;
		bool m_TickZenith;
		bool m_TickSpeed;
		bool m_TickGoal;
	};
}
#endif