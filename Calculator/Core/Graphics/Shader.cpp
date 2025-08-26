//
// Created by scion on 6/10/2025.
//

#include "Shader.h"

#include <fstream>
#include <iostream>
#include <Calculator/Core/Utility/Array.h>
#include <Calculator/Core/Utility/Binary.h>
#include <GLAD/glad.h>
#include <format>

ShaderStage::ShaderStage(u32 stage, const Path& path)
{
    ID = glCreateShader(stage);

    std::ifstream fileIn(path, std::ios::ate | std::ios::binary);
    const u64 fileLength = fileIn.tellg();
    fileIn.seekg(0);

    Array<char> source = Read<char>(fileIn, fileLength);

    const char* sources = source.Data();
    const i32 lengths = fileLength;

    glShaderSource(ID, 1, &sources, &lengths);
    glCompileShader(ID);

    i32 logLength;
    glGetShaderiv(ID, GL_COMPILE_STATUS, &logLength);

    if(logLength)
        return;

    glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &logLength);

    std::string log(logLength, 0);
    glGetShaderInfoLog(ID, logLength + 1, nullptr, log.data());

    std::cout << std::format("Shader \"{}\" compile failure:\n{}", path.string(), log) << std::endl;
}

ShaderStage::~ShaderStage()
{
    glDeleteShader(ID);
}

Shader::Shader(const Path& vertex, const Path& fragment)
{
    ID = glCreateProgram();

    const ShaderStage vert(GL_VERTEX_SHADER, vertex);
    const ShaderStage frag(GL_FRAGMENT_SHADER, fragment);

    Attach(vert);
    Attach(frag);

    glLinkProgram(ID);
}

void Shader::Bind() const
{
    glUseProgram(ID);
}

u32 Shader::GetUniform(const std::string_view name) const
{
    return glGetUniformLocation(ID, &*name.begin());
}

void Shader::SetUniform(i32 loc, float val) const
{
    glProgramUniform1f(ID, loc, val);
}

void Shader::SetUniform(i32 loc, u32 val) const
{
    glProgramUniform1ui(ID, loc, val);
}

void Shader::SetUniform(i32 loc, i32 val) const
{
    glProgramUniform1i(ID, loc, val);
}

void Shader::SetUniform(i32 loc, const Texture& texture, u32 bind) const
{
    texture.Bind(bind);
    SetUniform(loc, (i32) bind);
}

void Shader::Attach(const ShaderStage& stage) const
{
    glAttachShader(ID, stage.GetID());
}

Shader::~Shader()
{
    glDeleteProgram(ID);
}
