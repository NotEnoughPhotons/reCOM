#ifndef ZSHADER_H
#define ZSHADER_H
#include "gamez/zSystem/zsys.h"
#include "gamez/zMath/zmath.h"

#include "vector"

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

template<typename T>
struct shaderuniform_t
{
    const char* name;
    u32 location;
    T type;
};

class CShader
{
public:
    CShader();
    ~CShader();

    bool Create();
    void Use();
    void Destroy();

    bool LoadVertexShader(const char*);
    bool LoadFragmentShader(const char*);

    void SetInt(const char*, s32);
    void SetInt(const char*, u32);
    void SetFloat(const char*, f32);
    void SetVec2(const char*, PNT2D);
    void SetVec3(const char*, PNT3D);
    void SetVec4(const char*, PNT4D);
    void SetMat4(const char*, CMatrix);
    void SetTexture(const char*, u32);
    void SetCubemap(const char*, u32);

    u32 GetProgram() const { return m_program; }

    u32 GetVertexShader() const { return m_vsHandle; }
    u32 GetFragmentShader() const { return m_fsHandle; }
private:
    void CreateProgram();
    void Attach();
    void Link();
    void InitUniforms();

    u32 m_program;

    const char* m_vsSource;
    const char* m_fsSource;

    u32 m_vsHandle;
    u32 m_fsHandle;
};
#endif