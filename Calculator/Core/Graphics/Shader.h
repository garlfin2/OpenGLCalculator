//
// Created by scion on 6/10/2025.
//

#pragma once

#include "GL.h"
#include "Texture.h"

class ShaderStage : public GLObject
{
public:
    GLOBJECT_CONSTRUCTOR(ShaderStage);

    ShaderStage(u32 stage, const Path& path);

    ~ShaderStage() override;
};

class Shader : public GLObject
{
public:
    GLOBJECT_CONSTRUCTOR(Shader);

    Shader(const Path& vertex, const Path& fragment);

    void Bind() const;

    u32 GetUniform(std::string_view name) const;

    void SetUniform(i32, float) const;
    void SetUniform(i32, u32) const;
    void SetUniform(i32, i32) const;
    void SetUniform(i32, const Texture& texture, u32) const;

    template<class T>
    void SetUniform(std::string_view name, const T& value)
    {
        SetUniform(GetUniform(name), value);
    }

    ~Shader() override;

private:
    void Attach(const ShaderStage& stage) const;
};
