#include "ztwod.h"

#include "gamez/zTexture/ztex.h"

zdb::CCamera* C2D::m_p2dcamera;

f32 C2D::m_topx;
f32 C2D::m_botx;
f32 C2D::m_topy;
f32 C2D::m_boty;
f32 C2D::m_fXPixel;
f32 C2D::m_fYPixel;

C2D::C2D()
{
	m_on = true;
	first = true;
	m_isAA = false;
	m_isTrans = false;
	m_hasTexture = true;
	m_isFrameAlpha = false;
	
	m_uivar = NULL;
	m_active_and_handling_input = true;
}

void C2D::Init()
{
	m_topx = 1728.0f;
	m_botx = 2368.0f;
	m_topy = 1824.0f;
	m_boty = 2272.0f;
	m_fXPixel = 1.0f;
	m_fYPixel = 1.0f;
}

void C2D::Open()
{
	if (m_p2dcamera == NULL)
		m_p2dcamera = new zdb::CCamera();
}

bool C2D::CanHandleInput()
{
	return m_active_and_handling_input;
}

void C2D::SetCanHandleInput(bool enable)
{
	m_active_and_handling_input = enable;
}

void C2D::GetAssetLibs(CLibList& libs)
{
	for (auto i = begin(); i != end(); ++i)
		(*i)->GetAssetLibs(libs);
}

void C2D::Draw(zdb::CCamera* camera)
{
	if (!m_on)
		return;

	for (auto i = begin(); i != end(); ++i)
		(*i)->Draw(camera);
}

void C2D::Enable(bool enable)
{
	m_on = enable;
}

void C2D::SetUseFrameBufferAlpha(bool enable)
{
	m_isFrameAlpha = enable;
}

bool C2D::IsOn() const
{
	return m_on == true;
}

void C2D::On()
{
	m_on = true;
}

void C2D::Off()
{
	m_on = false;
}