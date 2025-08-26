#version 460

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 UV;
layout(location = 2) in vec3 Color;

struct Vertex
{
    vec2 Position;
    vec2 UV;
    vec3 Color;
};

out Vertex FragmentIn;

void main()
{
    FragmentIn.Position = Position * 2.0 - 1.0;
    FragmentIn.UV = vec2(UV.x, 1.0 - UV.y);
    FragmentIn.Color = Color;

    gl_Position = vec4(FragmentIn.Position, 0.0, 1.0);
}