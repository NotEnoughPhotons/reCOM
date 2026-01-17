#include "zanim.h"

bool zAnimQuadAlignTick(_zanim_cmd_hdr* header, f32* dT)
{
    return true;
}

bool zAnimIfTick(_zanim_cmd_hdr* header, f32* dT)
{
    ZAnim.m_CurSeq->m_IF_NestLevel++;

    while (!CZAnimExpression::ProcExpr(dT))
    {
        _zanim_cmd_hdr* header = NULL;
        bool evaluated = false;

        while (!evaluated)
        {
            if (!ZAnim.CmdValid())
            {
                evaluated = true;
                break;
            }

            if (ZAnim.m_CurSeq)
                header = reinterpret_cast<_zanim_cmd_hdr*>(ZAnim.m_CurSeq->cmd_pc);

            if (header->data_type != ZAnim.m_cmd_else.data_type)
                if (header->data_type != ZAnim.m_cmd_elseif.data_type)
                    if (header->data_type != ZAnim.m_cmd_endif.data_type)
                        continue;

            if (header->data_size == ZAnim.m_CurSeq->m_IF_NestLevel)
            {
                evaluated = true;
                if (header->data_type == ZAnim.m_cmd_endif.data_type)
                    ZAnim.m_CurSeq->m_IF_NestLevel--;
            }
        }

        if (!ZAnim.m_CurSeq)
            header = NULL;
        else
            header = reinterpret_cast<_zanim_cmd_hdr*>(ZAnim.m_CurSeq->cmd_pc);

        if (header->data_type != ZAnim.m_cmd_elseif.data_type)
            evaluated = true;
    }

    return true;
}

bool zAnimWhileTick(_zanim_cmd_hdr* header, f32* dT)
{
    if (header->data_type == 0)
    {
        ZAnim.CmdNext(8);
        if (CZAnimExpression::ProcExpr(dT))
        {
            ZAnim.CmdSet(header->data_size);
        }
    }

    return true;
}

bool zAnimLoopTick(_zanim_cmd_hdr* header, f32* dT)
{
    ZAnim.m_CurSeq->loop_counter++;
    ZAnim.m_CurSeq->loop_timer += *dT + ZAnim.m_CurSeq->seq_timer;

    if ((header->data_type & 1) == 0)
    {
        if ((header->data_type & 2) != 0
            && ((f32)header->data_size >= 0.0f)
            && (f32)header->data_size <= ZAnim.m_CurSeq->loop_timer)
        {
            return true;
        }
    }
    else if (header->data_size != -1 && header->data_size == ZAnim.m_CurSeq->loop_counter)
    {
        return true;
    }

    // ZAnim.SeqSetState(3);
    
    return false;
}

bool zAnimWaitTick(_zanim_cmd_hdr* header, f32* dT)
{
    bool status = false;
    f32 timer = 0.0f;
    
    if ((header->data_type & 8) != 0)
    {
        if ((header->data_type & 4) == 0)
        {
            if ((header->data_type & 2) == 0)
            {
                timer = ZAnim.m_CurSeq->cmd_timer;
            }
            else
            {
                timer = ZAnim.m_CurSeq->seq_timer;
            }
        }
        else
        {
            timer = ZAnim.m_CurAnim->m_timer;
        }

        if ((f32)header->data_size < timer + *dT)
        {
            timer = (f32)header->data_size - timer;

            if (timer < 0.0f)
            {
                timer = 0.0f;
            }

            status = true;
            *dT -= timer;
        }
        else
        {
            *dT = 0.0f;
            status = false;
        }
    }
    
    // TODO: add fucked bitfield header if check or whatever
    
    return status;
}

_zanim_cmd_hdr* CZAnimExpression::NewCmd(const char* operation)
{
    if (operation)
    {
        
    }

    return NULL;
}