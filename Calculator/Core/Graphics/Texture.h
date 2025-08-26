//
// Created by scion on 6/7/2025.
//

#pragma once

#include "GL.h"

#include <Calculator/Core/Utility/Math.h>
#include <Calculator/Core/Utility/Array.h>

struct TextureCompressionScheme
{
    u8 BlockSize = 1;
    u8 ByteSize = 1;

    GET_CONST(bool, IsCompressed, BlockSize != 1);

    NODISCARD u64 CalculateSize(Size2D size) const;
};

struct TextureSettings
{
    Size2D Size;
    u32 PixelFormat = 0;
    u32 PixelType = 0;
    u32 InternalFormat = 0;
    TextureCompressionScheme CompressionScheme = DEFAULT;
    u8 MipCount = 1;

    Array<std::byte> Data;
};

class Texture : public GLObject
{
public:
    GLOBJECT_CONSTRUCTOR(Texture);

    explicit Texture(const TextureSettings& settings);

    void Bind() const;
    void Bind(u32) const;

    GET_CONST(Size2D, Size, _size);

    ~Texture() override;

private:
    Size2D _size = DEFAULT;
};

namespace PVR
{
    enum class PVRFormat : u64
    {
        BC3 = 11,
        RGBAU8 = 578721384203708274,
        RU8 = 34359738482
    };

    void GetTextureSettings(PVRFormat format, TextureSettings& settings);
    Texture FromFile(const Path& path);
}
