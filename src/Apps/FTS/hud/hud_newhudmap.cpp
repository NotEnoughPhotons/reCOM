#include "hud.h"

std::map<s32, LINE_DATA*> tacmap_polys;
std::map<s32, LINE_DATA*> tacmap_lines;

bool hudAddPolyType(_zrdr* node)
{
    s32 type;
    LINE_DATA line;
    
    zrdr_findint(node, "Type", &type);

    tacmap_polys.insert({ type, &line });
    
    zrdr_findPNT3D(node, "Color", &line.color);
    zrdr_findreal(node, "Opacity", &line.opacity);
    line.opacity *= 128.0f;
    zrdr_findreal(node, "Width", &line._fWidth);

    return true;
}

bool hudAddLineType(_zrdr* node)
{
    s32 type;
    LINE_DATA line;
    
    zrdr_findint(node, "Type", &type);

    tacmap_lines.insert({ type, &line });
    
    zrdr_findPNT3D(node, "Color", &line.color);
    zrdr_findreal(node, "Opacity", &line.opacity);
    line.opacity *= 128.0f;
    zrdr_findreal(node, "Width", &line._fWidth);

    return true;
}