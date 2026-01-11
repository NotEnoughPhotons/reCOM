#include <fstream>
#include <sstream>

#include "zshader.h"

#include <GL\glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <SDL3/SDL.h>

CShader::CShader()
{
    m_program = 0;
    m_vsHandle = 0;
    m_fsHandle = 0;

    m_vsSource = nullptr;
    m_fsSource = nullptr;
}

CShader::~CShader()
{
    Destroy();
}

bool CShader::Create()
{
    CreateProgram();
    Attach();
    Link();

    return true;
}

void CShader::Use()
{
    glUseProgram(m_program);
}

void CShader::Destroy()
{
    glDeleteProgram(m_program);
    glDeleteShader(m_vsHandle);
    glDeleteShader(m_fsHandle);

    m_program = 0;
    m_vsHandle = 0;
    m_fsHandle = 0;

    delete m_vsSource;
    m_vsSource = nullptr;
    delete m_fsSource;
    m_fsSource = nullptr;
}

bool CShader::LoadVertexShader(const char* path)
{
    std::fstream file(path);

    if (!file.good())
        return false;

    std::stringstream buf;

    buf << file.rdbuf();

    m_vsSource = _strdup(buf.str().c_str());

    m_vsHandle = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_vsHandle, 1, &m_vsSource, nullptr);
    glCompileShader(m_vsHandle);

    s32 success = 0;
    glGetShaderiv(m_vsHandle, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(m_vsHandle, 512, nullptr, infoLog);
        SDL_Log("[ERROR] - Vertex error: %s", infoLog);
    }

    return true;
}

bool CShader::LoadFragmentShader(const char* path)
{
    std::fstream file(path);

    if (!file.good())
        return false;

    std::stringstream buf;

    buf << file.rdbuf();

    m_fsSource = _strdup(buf.str().c_str());

    m_fsHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_fsHandle, 1, &m_fsSource, nullptr);
    glCompileShader(m_fsHandle);

    s32 success = 0;
    glGetShaderiv(m_fsHandle, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(m_fsHandle, 512, nullptr, infoLog);
        SDL_Log("[ERROR] - Fragment error: %s", infoLog);
    }

    return true;
}

// TODO: Optimize these so we don't have to keep retrieving -
// uniforms from the shaders every time.
// Ideally we'd have a way to automatically get a list of uniforms,
// generated at the time the program starts.
// From there, we could just access the list whenever we want a uniform.

void CShader::SetFloat(const char* name, f32 value)
{
    s32 location = glGetUniformLocation(m_program, name);
    glUniform1f(location, value);
}

void CShader::SetVec2(const char* name, glm::vec2 value)
{
    s32 location = glGetUniformLocation(m_program, name);
    glUniform2f(location, value.x, value.y);
}

void CShader::SetVec3(const char* name, glm::vec3 value)
{
    s32 location = glGetUniformLocation(m_program, name);
    glUniform3f(location, value.x, value.y, value.z);
}

void CShader::SetVec4(const char* name, glm::vec4 value)
{
    s32 location = glGetUniformLocation(m_program, name);
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void CShader::SetMat4(const char* name, glm::mat4x4 value)
{
    s32 location = glGetUniformLocation(m_program, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void CShader::SetTexture(const char* name, u32 texture)
{
    s32 location = glGetUniformLocation(m_program, name);
    glUniform1i(location, texture);
}

void CShader::SetCubemap(const char* name, u32 cubemap)
{
    s32 location = glGetUniformLocation(m_program, name);
    glUniform1i(location, cubemap);
}

void CShader::CreateProgram()
{
    m_program = glCreateProgram();
}

void CShader::Attach()
{
    if (m_program == 0)
    {
        return;
    }

    glAttachShader(m_program, m_vsHandle);
    glAttachShader(m_program, m_fsHandle);
}

void CShader::Link()
{
    if (m_program == 0)
    {
        return;
    }

    glLinkProgram(m_program);
}

void CShader::InitUniforms()
{

}