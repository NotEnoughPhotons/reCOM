#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_joystick.h>

#include "gamez/zInput/zinput.h"
#include "gamez/zMath/zmath.h"

float pad_dt = 0.0f;

bool CInput::m_init = false;
CPad* CInput::m_pads[2];
CKeyboard* CInput::m_keyboard = NULL;

void CInput::Init()
{
	m_init = OpenPadIO() != false;
	CKeyboard::Init();
	m_init = true;
}

int CInput::OpenPadIO()
{
	return 1;
}

void CInput::ClosePadIO()
{
	for (int i = 0; i < 2; i++)
	{
		DeletePad(i);
	}
}

void CInput::Tick(f32 dT)
{
	if (m_keyboard != NULL)
	{
		m_keyboard->Tick(dT);
	}

	for (s32 pad = 0; pad < 2; pad++)
	{
		if (m_pads[pad] != NULL)
		{
			m_pads[pad]->Tick(dT);
		}
	}
}

CPad* CInput::CreatePad(u32 slot)
{
	CPad* pad;

	if (slot < 2)
	{
		pad = m_pads[slot];

		if (pad == NULL)
		{
			pad = new CPad(slot, 0);
		}

		m_pads[slot] = pad;
		
		if (!pad->IsOpen() && m_pads[slot] != NULL)
		{
			
		}
	}
	else
	{
		return NULL;
	}

	return pad;
}

void CInput::DeletePad(u32 slot)
{
	CPad* pad = m_pads[slot];

	if (slot < 2 && pad != NULL)
	{
		// delete pad;
		m_pads[slot] = NULL;
	}
}

void CInput::Flush()
{
	for (s32 pad = 0; pad < 2; pad++)
	{
		if (m_pads[pad] != NULL)
		{
			m_pads[pad]->Flush();
		}
	}
}

void CInput::Uninit()
{
	ClosePadIO();
	m_init = false;
}