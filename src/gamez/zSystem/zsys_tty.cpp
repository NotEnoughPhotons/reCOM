#include "zsys.h"

#include "gamez/zVideo/zvid.h"

CTTY theTerminal;

void CTTY::Print(char* string)
{
    Print(string, strlen(string));
}

void CTTY::Print(char* buffer, size_t size)
{
    memset(m_buffer, 0, 1024);
    strcpy_s(m_buffer, size, buffer);
}

void CTTY::Render()
{
    
}