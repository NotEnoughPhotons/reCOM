#include "zcam.h"

#include "Apps/FTS/hud/hud.h"

#include "gamez/zSeal/zseal.h"

CHUD* CameraZoom::m_hud = NULL;
CZSealBody* CameraZoom::m_seal = NULL;

void CameraZoom::AttachHud(CHUD* hud)
{
	m_hud = hud;
}

void CameraZoom::AttachSeal(CZSealBody* seal)
{
	m_seal = seal;
}