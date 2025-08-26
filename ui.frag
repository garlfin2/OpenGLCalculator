#version 460

struct Vertex
{
    vec2 Position;
    vec2 UV;
    vec3 Color;
};

in Vertex FragmentIn;

out vec4 FragColor;

layout(location = 0) uniform sampler2D Color;
layout(location = 1) uniform bool OneChannel;

void main()
{
    FragColor = texture(Color, FragmentIn.UV);

    if(OneChannel)
    {
        FragColor.a = FragColor.r;
        FragColor.rgb = vec3(1.0);
    }

    FragColor.rgb *= FragmentIn.Color;
}