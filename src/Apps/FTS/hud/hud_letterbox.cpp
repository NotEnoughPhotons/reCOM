#include "hud.h"
#include "gamez/zVideo/zvid.h"

CLetterBox::CLetterBox()
{
    for (u32 i = 0; i < 2; i++)
    {
        m_bitmap[i].m_hasTexture = false;
        m_bitmap[i].Load(0.0f, 0.0f, 1.0f, 1.0f, NULL);
        m_bitmap[i].SetColor(0.0f, 0.0f, 0.0f);
        m_bitmap[i].SetTrans(128.0f);
        m_bitmap[i].m_isTrans = true;
        m_bitmap[i].On();
        insert(begin(), &m_bitmap[i]);
    }

    m_curpos = 0.0f;
}

void CLetterBox::AddLetterbox()
{
    m_slidedir = SLIDE_IN;
}

void CLetterBox::RemoveLetterbox()
{
    m_slidedir = SLIDE_OUT;
}

void CLetterBox::Draw(zdb::CCamera* camera)
{
    m_bitmap[0].Draw(camera);
    m_bitmap[1].Draw(camera);
}

void CLetterBox::Tick(f32 dT)
{
    if (m_slidedir == SLIDE_OUT)
    {
        if (m_curpos == 0.0f)
        {
            m_slidedir = SLIDE_NONE;
        }
        else
        {
            f32 pos = m_curpos - dT * 128.0f;
            m_curpos = pos;

            if (pos <= 0.0f)
            {
                m_curpos = 0.0f;
                m_slidedir = SLIDE_NONE;

                // m_bitmap[0].TickFade();
                // m_bitmap[1].TickFade();
            }
            
            m_bitmap[0].Load(0.0f, 0.0f, theWindow->GetWidth(), m_curpos, NULL);
            m_bitmap[1].Load(0.0f, theWindow->GetHeight(), theWindow->GetWidth(), theWindow->GetHeight() - m_curpos, NULL);
        }
    }
    else if (m_slidedir == SLIDE_IN)
    {
        if (m_curpos >= 64.0f)
        {
            m_slidedir = SLIDE_NONE;
        }
        else
        {
            f32 pos = m_curpos + dT * 128.0f;
            m_curpos = pos;

            if (64.0f <= pos)
            {
                m_curpos = 64.0f;
                m_slidedir = SLIDE_NONE;
            }

            m_bitmap[0].Load(0.0f, 0.0f, theWindow->GetWidth(), m_curpos, NULL);
            m_bitmap[1].Load(0.0f, theWindow->GetHeight(), theWindow->GetWidth(), theWindow->GetHeight() - m_curpos, NULL);
        }
    }
}
