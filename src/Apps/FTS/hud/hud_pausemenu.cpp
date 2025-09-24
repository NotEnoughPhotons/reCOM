#include "hud.h"
#include "hud.h"
#include "gamez/zSeal/zseal.h"
#include "gamez/zVideo/zvid.h"

CZPauseTest::CZPauseTest()
{
    m_numselections = 0;
    m_curcolor = 0.0f;
    m_inc = 50.0f;
    m_font = NULL;
    m_oldmenustate = stateNone;
    m_selectsound = NULL;
    m_scrollupsound = NULL;
    m_scrolldownsound = NULL;
}

void CZPauseTest::ReInit()
{
    m_ttf_font = new C2DTTFFont();
    m_ttf_font->Load("Z:/ARIALNB.ttf");
    
    m_numselections = 5;
    
    m_selections[0].Load(1.2f, "RESUME", m_ttf_font, 240, 200);
    m_selections[0].m_color.x = 86.0f;
    m_selections[0].m_color.y = 63.0f;
    m_selections[0].m_color.z = 27.0f;

    m_selections[1].Load(1.2f, "ABORT", m_ttf_font, 240, 232);
    m_selections[1].m_color.x = 86.0f;
    m_selections[1].m_color.y = 63.0f;
    m_selections[1].m_color.z = 27.0f;

    m_selections[2].Load(1.2f, "INVERT PITCH: OFF", m_ttf_font, 240, 264);
    m_selections[2].m_color.x = 86.0f;
    m_selections[2].m_color.y = 63.0f;
    m_selections[2].m_color.z = 27.0f;

    m_selections[3].Load(1.2f, "AUTO AIM: OFF", m_ttf_font, 240, 296);
    m_selections[3].m_color.x = 86.0f;
    m_selections[3].m_color.y = 63.0f;
    m_selections[3].m_color.z = 27.0f;

    m_selections[4].Load(1.2f, "REPLAY", m_ttf_font, 240, 328);
    m_selections[4].m_color.x = 86.0f;
    m_selections[4].m_color.y = 63.0f;
    m_selections[4].m_color.z = 27.0f;
}

void CZPauseTest::Draw(zdb::CCamera* camera)
{
    for (u32 i = 0; i < m_numselections; i++)
    {
        m_selections[i].Draw(NULL);
    }
}

void CZPauseTest::OpenPauseMenu()
{
    CZSealBody* owner = CHUD::GetHUDOwner();
    CSealCtrl* control = NULL;
    CPad* player_pad = NULL;
    
    if (owner)
    {
        control = static_cast<CSealCtrl*>(owner->m_control);
    }
    
    control->StopRumbler();

    if (control->m_padid < 2)
    {
        player_pad = CInput::m_pads[control->m_padid];
    }

    player_pad->SetButtonState(PAD_BUTTON::PAD_START, KEY_DOWN);
    CSndInstance::PauseAllSounds();
    CSnd::PostTick(0.0f, NULL);

    On();

    theHUD->m_letterbox.AddLetterbox();

    if (theHUD->m_newmap.m_isFullScreen)
    {
        theHUD->m_newmap.SwitchToCompassMap();
    }

    theHUD->Off();

    m_oldmenustate = control->GetMenuState();
    control->SetMenuState(statePauseTestMenu);
}
