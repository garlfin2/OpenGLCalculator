//
// Created by scion on 6/10/2025.
//

#include "VAO.h"

#include <glad/glad.h>

DynamicBuffer::DynamicBuffer(const u64 size)
{
    glCreateBuffers(1, &ID);

    _vertices.reserve(std::max<u64>(size, 4));

    Update();
}

void DynamicBuffer::Update()
{
    if(_bufferSize != _vertices.capacity())
    {
        _bufferSize = _vertices.capacity();
        glNamedBufferData(ID, (GLsizeiptr)(sizeof(ColoredVertex) * _bufferSize), _vertices.data(), GL_DYNAMIC_DRAW);
        return;
    }

    glNamedBufferSubData(ID, 0, (GLsizeiptr)(sizeof(ColoredVertex) * _vertices.size()), _vertices.data());
}

void DynamicBuffer::Clear()
{
    _vertices = std::vector<ColoredVertex>();
    _vertices.reserve(4);
}

DynamicBuffer::~DynamicBuffer()
{
    glDeleteBuffers(1, &ID);
}

DynamicVAO::DynamicVAO(const u64 size) : _buffer(size)
{
    glCreateVertexArrays(1, &ID);

    glVertexArrayVertexBuffer(ID, 0, _buffer.GetID(), 0, sizeof(ColoredVertex));

    glEnableVertexArrayAttrib(ID, 0);
    glEnableVertexArrayAttrib(ID, 1);
    glEnableVertexArrayAttrib(ID, 2);

    glVertexArrayAttribFormat(ID, 0, 2, GL_FLOAT, false, offsetof(ColoredVertex, Position));
    glVertexArrayAttribFormat(ID, 1, 2, GL_FLOAT, false, offsetof(ColoredVertex, UV));
    glVertexArrayAttribFormat(ID, 2, 3, GL_FLOAT, false, offsetof(ColoredVertex, Color));

    glVertexArrayAttribBinding(ID, 0, 0);
    glVertexArrayAttribBinding(ID, 1, 0);
    glVertexArrayAttribBinding(ID, 2, 0);
}

void DynamicVAO::Draw() const
{
    Bind();
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(_buffer.GetVertices().size()));
}

void DynamicVAO::Clear()
{
    _buffer.Clear();
}

void DynamicVAO::PushQuad(const Quad& quad, const vec3& color)
{
    // c -- d
    // |    |
    // a -- b

    const Vertex& a = quad.Min;
    const Vertex b { vec2(quad.Max.Position.x, quad.Min.Position.y), vec2(quad.Max.UV.x, quad.Min.UV.y) };
    const Vertex c { vec2(quad.Min.Position.x, quad.Max.Position.y), vec2(quad.Min.UV.x, quad.Max.UV.y) };
    const Vertex& d = quad.Max;

    _buffer.Push(a, color);
    _buffer.Push(b, color);
    _buffer.Push(d, color);

    _buffer.Push(d, color);
    _buffer.Push(c, color);
    _buffer.Push(a, color);
}

void DynamicVAO::Bind() const
{
    glBindVertexArray(ID);
}

void DynamicVAO::Update()
{
    _buffer.Update();
}

DynamicVAO::~DynamicVAO()
{
    glDeleteVertexArrays(1, &ID);
}
