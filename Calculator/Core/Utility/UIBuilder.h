//
// Created by scion on 6/11/2025.
//

#pragma once

#include <Calculator/Core/Utility/Math.h>
#include <Calculator/Core/Graphics/Texture.h>
#include <Calculator/Core/Graphics/VAO.h>

class Window;
struct CalculatorButton;

using CalculatorButtonFunc = void(*)(Window*, const CalculatorButton&);
struct CalculatorButton
{
    CalculatorButton(Size2D position, std::string_view text, CalculatorButtonFunc onClick, Size2D size = { 1, 1 }) :
        Position(position),
        Size(size),
        Text(text),
        OnClick(onClick)
    {}

    CalculatorButton() = default;

    Size2D Position = { 0, 0 };
    Size2D Size { 1, 1 };
    std::string_view Text = DEFAULT;
    CalculatorButtonFunc OnClick = DEFAULT;
    vec3 Color = vec3(1.f);
};

class ButtonFactory
{
public:
    ButtonFactory() = delete;

    template<typename... ARGS>
    static CalculatorButton CreateButton(ARGS&&... args)
    {
        return CalculatorButton{ std::forward<ARGS>(args)... };
    }
};

namespace UIBuilder
{
    struct NineSlice
    {
        // x-----x
        // | x-x |
        // | x-x |
        // x-----x

        // Offset: side to inner wall
        float Offset = 0.125;

        // CornerSize: How big a corner will be.
        float CornerSize = 0.125;
    };

    struct Glyph
    {
        float Advance;
        Quad Quad;
    };

    struct Font
    {
        float LineHeight;

        Texture Texture;
        Glyph Glyphs[UINT8_MAX];
    };

    Font LoadFont(const Path& json, const Path& texture);

    void Build9Slice(DynamicVAO& vao, Position2D min, Position2D max, const NineSlice& nineSlice, const vec3& color = vec3(1.f));
    void BuildText(DynamicVAO& vao, Position2D position, float scale, std::string_view text, const Font& font, const vec3& color = vec3(0.f));
    void BuildButton(Window& window, const CalculatorButton& button);

    Position2D GetTextSize(float scale, std::string_view text, const Font& font);
}
