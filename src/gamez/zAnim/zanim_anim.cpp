#include "zanim.h"

u32* seq_offset_stack = NULL;
u32 seq_offset_stack_size = 0;

_zanim_cmd_hdr* zAnimLoad_If(_zrdr* tag)
{
    _zanim_cmd_hdr* cmd = NULL;

    if (tag)
    {
        cmd = (_zanim_cmd_hdr*)zcalloc(1, 8);
        cmd->data_type &= ANIMTYPE_CONDITION;
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
        cmd->data_type &= ANIMTYPE_CONDITION;
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
        cmd->data_type &= ANIMTYPE_CONDITION;
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
        cmd->data_type &= ANIMTYPE_CONDITION;
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
        cmd->data_type &= ANIMTYPE_CONDITION;

        if (tag->type == ZRDR_ARRAY && tag->array->integer == ANIMTYPE_CONDITION)
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