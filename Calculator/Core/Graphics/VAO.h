//
// Created by scion on 6/10/2025.
//

#pragma once

#include "GL.h"

#include <vector>

struct Vertex
{
    vec2 Position;
    vec2 UV;

    Vertex operator+(vec2 position) const { return { Position + position, UV }; }
    Vertex operator*(float scale) const { return { Position * scale, UV }; }
};

struct ColoredVertex
{
    ColoredVertex(const Vertex& vertex, const vec3& color) : Position(vertex.Position), UV(vertex.UV), Color(color) {}
    vec2 Position;
    vec2 UV;
    vec3 Color;
};

struct Quad
{
    Vertex Min, Max;

    void Fix()
    {
        FixMinMax(Min.Position, Max.Position);
        FixMinMax(Min.UV, Max.UV);
    }

    Quad operator+(vec2 position) const { return Quad{ Min + position, Max + position }; }
    Quad operator*(float scale) const { return Quad{ Min * scale, Max * scale }; }
};

class DynamicBuffer : public GLObject
{
public:
    GLOBJECT_CONSTRUCTOR(DynamicBuffer);

    explicit DynamicBuffer(u64 size);

    GET_CONST(const std::vector<ColoredVertex>&, Vertices, _vertices);

    template<typename... ARGS>
    void Push(ARGS&&... args)
    {
        _vertices.emplace_back(std::forward<ARGS>(args)...);
    }

    void Update();
    void Clear();

    ~DynamicBuffer() override;

private:
    u64 _bufferSize = 0;
    std::vector<ColoredVertex> _vertices;
};

class DynamicVAO : public GLObject
{
public:
    GLOBJECT_CONSTRUCTOR(DynamicVAO);

    explicit DynamicVAO(u64 size);

    void Draw() const;
    void Clear();
    void PushQuad(const Vertex& min, const Vertex& max, const vec3& color) { PushQuad(Quad(min, max), color); }
    void PushQuad(const Quad& quad, const vec3& color);
    void Bind() const;
    void Update();

    ~DynamicVAO() override;

private:
    DynamicBuffer _buffer;
};
