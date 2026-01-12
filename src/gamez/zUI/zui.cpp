#include "zui.h"

CUIVarManager theUIVarManager;

s32 ZuiInit()
{
    CRdrFile* params = zrdr_read("UiParams.rdr", "data/common");

    return zrdr_free(params);
}

void CUIVariable::Set(_zrdr* reader)
{
    
}

CUIVariable* CUIVarManager::Add(const char* name, _zrdr* reader, UIVAR_LONGEVITY longevity)
{
    return NULL;
}