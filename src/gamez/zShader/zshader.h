#ifndef ZSHADER_H
#define ZSHADER_H
#include "gamez/zSystem/zsys.h"

class CShader
{
public:
    CShader();

    void Init();
    void Link();
    
    void Create(const char* path, u32& shader, const u32& type);
    
    void Bind();
    void Unbind();

    static u32 m_program;

    u32 m_ID;
    u32 m_vertex;
    u32 m_fragment;
};
#endif