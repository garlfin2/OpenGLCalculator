//
// Created by scion on 6/7/2025.
//

#include "Texture.h"

#include <fstream>
#include <Calculator/Core/Utility/Binary.h>
#include <glad/glad.h>

u64 TextureCompressionScheme::CalculateSize(Size2D size) const
{
    const Size2D blockCount = DivCeil(size, Size2D(BlockSize));
    return blockCount.x * blockCount.y * ByteSize;
}

void PVR::GetTextureSettings(const PVRFormat format, TextureSettings& settings)
{
    switch(format)
    {
    case PVRFormat::BC3:
        settings.InternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        settings.CompressionScheme = TextureCompressionScheme(4, 16);
        break;
    case PVRFormat::RGBAU8:
        settings.InternalFormat = GL_RGBA8;
        settings.PixelFormat = GL_RGBA;
        settings.PixelType = GL_UNSIGNED_BYTE;
        settings.CompressionScheme = TextureCompressionScheme(1, 4);
        break;
    case PVRFormat::RU8:
        settings.InternalFormat = GL_R8;
        settings.PixelFormat = GL_RED;
        settings.PixelType = GL_UNSIGNED_BYTE;
        settings.CompressionScheme = TextureCompressionScheme(1, 1);
        break;
    default:
        break;
    }
}

Texture::Texture(const TextureSettings& settings) : _size(settings.Size)
{
    glCreateTextures(GL_TEXTURE_2D, 1, &ID);

    glTextureStorage2D(ID, settings.MipCount, settings.InternalFormat, _size.x, _size.y);

    glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    u64 offset = 0;
    Size2D mipSize = _size;
    for(u32 i = 0; i < settings.MipCount; i++)
    {
        const u64 byteSize = settings.CompressionScheme.CalculateSize(mipSize);

        if(settings.CompressionScheme.GetIsCompressed())
            glCompressedTextureSubImage2D(ID, (GLint) i, 0, 0, (GLsizei) mipSize.x, (GLsizei) mipSize.y, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, (GLsizei) byteSize, &settings.Data[offset]);
        else
            glTextureSubImage2D(ID, (GLint) i, 0, 0, (GLsizei) mipSize.x, (GLsizei) mipSize.y, settings.PixelFormat, settings.PixelType, &settings.Data[offset]);
        offset += byteSize;
        mipSize = max(mipSize >> 1u, Size2D(1));
    }
}

Texture PVR::FromFile(const Path& path)
{
    std::ifstream fileIn(path, std::ios::binary);
    assert(fileIn.is_open());

    TextureSettings settings = DEFAULT;

    const u32 magic = Read<u32>(fileIn);
    assert(magic == 0x03525650); // Incorrect magic

    const u32 flags = Read<u32>(fileIn);

    const PVRFormat format = Read<PVRFormat>(fileIn);
    assert(format == PVRFormat::BC3 || format == PVRFormat::RGBAU8 || format == PVRFormat::RU8);
    // Only supports BC3 or RGBAU8 for simplicity.

    GetTextureSettings(format, settings);

    const u32 colorSpace = Read<u32>(fileIn);
    const u32 channelType = Read<u32>(fileIn);

    // Height before width for some reason
    settings.Size.y = Read<u32>(fileIn);
    settings.Size.x = Read<u32>(fileIn);

    const u32 sizeZ = Read<u32>(fileIn),
              surfaceCount = Read<u32>(fileIn),
              faceCount = Read<u32>(fileIn);

    settings.MipCount = Read<u32>(fileIn);

    const u32 metaDataSize = Read<u32>(fileIn);

    assert(sizeZ == 1);
    assert(surfaceCount == 1 && faceCount == 1);

    fileIn.seekg(metaDataSize, std::ios::cur);

    const u64 dataStart = fileIn.tellg();
    fileIn.seekg(0, std::ios::end);
    const u64 dataSize = (u64) fileIn.tellg() - dataStart;
    fileIn.seekg((i64) dataStart);

    settings.Data = Array<std::byte>(dataSize);
    fileIn.read((char*) settings.Data.Data(), (i64) dataSize);

    return Texture(settings);
}

void Texture::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Bind(u32 loc) const
{
    glBindTextureUnit(loc, ID);
}

Texture::~Texture()
{
    glDeleteTextures(1, &ID);
}
