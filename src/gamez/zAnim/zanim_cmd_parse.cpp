#include "zanim.h"

#include "gamez/zSystem/zsys.h"

bool CZAnimExpression::ProcExpr(f32* dT)
{
    s8* program_counter = NULL;
    
    if (!ZAnim.m_CurSeq)
    {
        return false;
    }
    else
    {
        program_counter = ZAnim.m_CurSeq->cmd_pc;
    }

    do
    {
        
    }
    while (true);

    return true;
}

bool CZAnimExpression::IsOperator(const char* operation)
{
    bool is_not = strcmp(operation, "!") == 0;
    bool is_and = strcmp(operation, "&&") == 0;
    bool is_or = strcmp(operation, "||") == 0;
    bool is_start_tag = strcmp(operation, "(") == 0;
    bool is_end_tag = strcmp(operation, ")") == 0;
    
    if (is_not || is_and || is_or || is_start_tag || is_end_tag)
    {
        return true;
    }

    return false;
}

_zanim_cmd_hdr* CZAnimMain::AnimParseExpression(_zanim_cmd_hdr* header, _zrdr* reader)
{
    if (!reader || reader->type != ZRDR_ARRAY)
    {
        header = NULL;
        return NULL;
    }

    u32 i = 0;
    
    while (true)
    {
        _zrdr* node = NULL;
        u32 reader_length = 0;
        char* string = NULL;
        
        if (reader->type == ZRDR_ARRAY)
        {
            reader_length = reader->array->length - 1;
        }

        if (reader_length <= i)
        {
            break;
        }

        reader_length = 0;

        if (reader->type == ZRDR_ARRAY)
        {
            reader_length = reader->array->length - 1;
        }

        if (i < reader_length)
        {
            node = &reader->array[i] + 1;
        }
        else
        {
            node = NULL;
        }

        if (node->type == ZRDR_STRING)
        {
            reader_length = 0;

            if (reader->type == ZRDR_ARRAY)
            {
                reader_length = reader->array->length - 1;
            }

            if (i < reader_length)
            {
                node = &reader->array[i] + 1;
            }
            else
            {
                node = NULL;
            }

            if (node->type == ZRDR_STRING)
            {
                string = node->string;
            }
            else
            {
                string = NULL;
            }

            if (CZAnimExpression::IsOperator(string))
            {
                reader_length = 0;

                if (reader->type == ZRDR_ARRAY)
                {
                    reader_length = reader->array->length - 1;
                }

                if (i + 1 < reader_length)
                {
                    node = &reader->array[i] + 2;
                }
                else
                {
                    node = NULL;
                }

                reader_length = 0;

                if (reader->type == ZRDR_ARRAY)
                {
                    reader_length = reader->array->length - 1;
                }

                if (i < reader_length)
                {
                    node = &reader->array[i] + 1;
                }
                else
                {
                    node = NULL;
                }

                string = NULL;
                if (node->type == ZRDR_STRING)
                {
                    string = node->string;
                }

                // AnimParseCmd()
                i++;
            }
        }
    }
}