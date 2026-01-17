#include "zanim.h"

u32* seq_offset_stack = NULL;
u32 seq_offset_stack_size = 0;

_zanim_cmd_hdr* zAnimLoad_If(_zrdr* tag)
{
    _zanim_cmd_hdr* cmd = NULL;

    if (tag)
    {
        cmd = (_zanim_cmd_hdr*)zcalloc(1, 8);
        cmd->data_type &= DATATYPE_IF;
        cmd = ZAnim.AnimParseExpression(cmd, tag);
    }

    ZAnim.m_CurSeq->m_IF_NestLevel++;
    cmd->data_size = ZAnim.m_CurSeq->m_IF_NestLevel;
    return cmd;
}

_zanim_cmd_hdr* zAnimLoad_Else(_zrdr* tag)
{
    _zanim_cmd_hdr* cmd = NULL;
    
    if (tag)
    {
        cmd = (_zanim_cmd_hdr*)zcalloc(1, 8);
        cmd->data_type &= DATATYPE_IF;
    }

    cmd->data_size = ZAnim.m_CurSeq->m_IF_NestLevel;
    return cmd;
}

_zanim_cmd_hdr* zAnimLoad_ElseIf(_zrdr* tag)
{
    _zanim_cmd_hdr* cmd = NULL;

    if (tag)
    {
        cmd = (_zanim_cmd_hdr*)zcalloc(1, 8);
        cmd->data_type &= DATATYPE_IF;
        cmd = ZAnim.AnimParseExpression(cmd, tag);
    }

    cmd->data_size = ZAnim.m_CurSeq->m_IF_NestLevel;
    return cmd;
}

_zanim_cmd_hdr* zAnimLoad_EndIf(_zrdr* tag)
{
    _zanim_cmd_hdr* cmd = NULL;

    if (tag)
    {
        cmd = (_zanim_cmd_hdr*)zcalloc(1, 8);
        cmd->data_type &= DATATYPE_END_IF;
    }

    cmd->data_size = ZAnim.m_CurSeq->m_IF_NestLevel;
    ZAnim.m_CurSeq->m_IF_NestLevel--;

    return cmd;
}

_zanim_cmd_hdr* zAnimLoadWhile(_zrdr* tag)
{
    _zanim_cmd_hdr* cmd = NULL;
    
    if (tag)
    {
        cmd = (_zanim_cmd_hdr*)zcalloc(1, 8);
        cmd->data_type &= DATATYPE_WHILE;

        if (tag->type == ZRDR_ARRAY && tag->array->integer == DATATYPE_IF)
        {
            _zrdr* evaluation = NULL;

            if (tag->array->integer != 1)
            {
                evaluation = &tag->array[1];
            }

            bool isTrue = false;
            if (evaluation->type == ZRDR_STRING)
            {
                isTrue = SDL_strcmp(evaluation->string, "true") == 0;
            }

            if (isTrue)
            {
                cmd->timeless = true;
                seq_offset_stack = (u32*)zrealloc(seq_offset_stack, (seq_offset_stack_size + 1) * sizeof(u32));

                if (ZAnim.m_CurSeq)
                {
                    seq_offset_stack[seq_offset_stack_size] = ZAnim.m_CurSeq->seq_data_size;
                }

                seq_offset_stack_size++;
                return cmd;
            }
        }

        cmd = ZAnim.AnimParseExpression(cmd, tag);
        
        seq_offset_stack = (u32*)zrealloc(seq_offset_stack, (seq_offset_stack_size + 1) * sizeof(u32));

        if (ZAnim.m_CurSeq)
        {
            seq_offset_stack[seq_offset_stack_size] = ZAnim.m_CurSeq->seq_data_size;
        }

        seq_offset_stack_size++;
        return cmd;
    }
}

void CZAnimSet::Name(const char* name)
{
    if (m_name)
        zfree(m_name);

    m_name = zstrdup(name);
}

bool CZAnimSet::Start()
{
    return false;
}

void CZAnimSet::Init()
{
    m_WasStarted = false;
    m_IsStarted = false;
    m_IsAvailable = true;
    m_name = NULL;
    m_si_script_count = 0;
    m_si_script = NULL;
    m_AnimExList = NULL;
    m_anim_count = 0;
    m_anim_max = 0;
    m_anim_list = NULL;
    m_node_states = NULL;
    m_task_scheduler = NULL;
}