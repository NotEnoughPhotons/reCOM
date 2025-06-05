#pragma once
#include "gamez/zAnim/zanim.h"

enum PARTICLE_TYPE
{
	PARTICLE_STANDARD,
	PARTICLE_ROTATED,
	PARTICLE_XZ,
	PARTICLE_STREAKED,
	NUM_PARTICLE_TYPES
};

class Particle
{
public:
	class Props
	{
	public:
		s32 m_refCount;

		Sequence m_textureSeq;
		Sequence m_colorSeq;
		Sequence m_scaleSeq;

		zdb::CModel* m_model;
		f32 m_friction;
		CPnt4D m_accelW;
		f32 m_windFactor;
		f32 m_priority;
		f32 m_nearFade1;
		f32 m_nearFade2;
		f32 m_farFade1;
		f32 m_farFade2;
		f32 m_invNearFade;
		f32 m_invFarFade;
		f32 m_visualDensity;
		bool m_checkFade;
	};

	class Source
	{
	public:
		Source();
		~Source();

		static Source* Create();

		void RenderBBox();
		
		void GetBBox(CBBox* box);
		void Default();
		
		void CheckExtents(CPnt4D extents, f32 padding);
		void ColorSequenceAdd(f32 param_1, f32 param_2, f32 param_3, f32 param_4, f32 param_5);
		void ColorSequenceClear();

		void Tick(f32 dT);
		void Reset();
	};
	
	class Instance
	{
	public:
		Instance(CPnt4D* param_1, CPnt4D* param_2, CPnt4D* param_3, f32 param_4,
			     f32 param_5, f32 param_6, zdb::CTexHandle*, Props* properties, Source* source);

		void Expire();
		void Render(const CMatrix& transform);
	};
	
	static void Open();
	static void Close();

	static void FlushDynTexList();
	
};