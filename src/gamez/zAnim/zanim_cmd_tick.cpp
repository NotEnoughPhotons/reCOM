#include "zanim.h"

bool zAnimIfTick(_zanim_cmd_hdr* header, f32* dT)
{
    ZAnim.m_CurSeq->m_IF_NestLevel++;

    do
    {
        ZAnim.CmdNext(8);

        if (!CZAnimExpression::ProcExpr(dT))
        {
            bool valid = false;
            do
            {
                ZAnim.CmdNext();
                
                if (ZAnim.CmdValid())
                {
                    u8* program_counter = NULL;

                    if (ZAnim.m_CurSeq)
                    {
                        program_counter = (u8*)ZAnim.m_CurSeq->cmd_pc;
                    }

                    if (*program_counter != ZAnim.m_cmd_else.data_size)
                    {
                        program_counter = NULL;

                        if (ZAnim.m_CurSeq)
                        {
                            program_counter = (u8*)ZAnim.m_CurSeq->cmd_pc;
                        }

                        if (*program_counter != ZAnim.m_cmd_elseif.data_size)
                        {
                            program_counter = NULL;

                            if (ZAnim.m_CurSeq)
                            {
                                program_counter = (u8*)ZAnim.m_CurSeq->cmd_pc;
                            }

                            if (*program_counter != ZAnim.m_cmd_endif.data_size)
                            {
                                continue;
                            }
                        }
                    }

                    program_counter = NULL;

                    if (ZAnim.m_CurSeq)
                    {
                        program_counter = (u8*)ZAnim.m_CurSeq->cmd_pc;
                    }

                    if (program_counter[4] == ZAnim.m_CurSeq->loop_counter)
                    {
                        valid = true;

                        if (!ZAnim.m_CurSeq)
                        {
                            program_counter = NULL;
                        }
                        else
                        {
                            program_counter = (u8*)ZAnim.m_CurSeq->cmd_pc;
                        }

                        if (*program_counter == ZAnim.m_cmd_endif.data_size)
                        {
                            ZAnim.m_CurSeq->loop_counter--;
                        }
                    }
                }
                else
                {
                    valid = true;
                }
            } while (!valid);

            u8* program_counter = NULL;
            
            if (!ZAnim.m_CurSeq)
            {
                program_counter = NULL;
            }
            else
            {
                program_counter = (u8*)ZAnim.m_CurSeq->cmd_pc;
            }

            if (*program_counter != ZAnim.m_cmd_elseif.data_size)
            {
                return true;
            }
        }
        else
        {
            return true;
        }
    } while (true);
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