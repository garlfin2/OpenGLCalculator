//
// Created by scion on 6/11/2025.
//

#include "UIBuilder.h"

#include <fstream>
#include <Vendor/JSON/json.hpp>
#include <Calculator/Core/Window.h>

#define TEXT_SCALE 0.05f

UIBuilder::Font UIBuilder::LoadFont(const Path& jsonPath, const Path& texturePath)
{
    using json = nlohmann::json;
    using object = nlohmann::basic_json<>;

    std::ifstream jsonIn(jsonPath);
    json file = json::parse(jsonIn);

    Font result;
    result.Texture = PVR::FromFile(texturePath);

    object atlas = file["atlas"];
    Size2D atlasSize = { atlas["width"], atlas["height"] };

    object metrics = file["metrics"];
    result.LineHeight = (float) metrics["lineHeight"];

    for(object glyph : file["glyphs"])
    {
        Glyph& glyphOut = result.Glyphs[(u64) glyph["unicode"]];

        glyphOut.Advance = (float) glyph["advance"];

        if(!glyph.contains("planeBounds"))
            continue;

        object planeBounds = glyph["planeBounds"];
        object uvBounds = glyph["atlasBounds"];

        glyphOut.Quad.Min = Vertex
        {
            vec2((float) planeBounds["left"], (float) planeBounds["bottom"]),
            vec2((float) uvBounds["left"] / atlasSize.x, (float) uvBounds["bottom"] / atlasSize.y)
        };

        glyphOut.Quad.Max = Vertex
        {
            vec2((float) planeBounds["right"], (float) planeBounds["top"]),
            vec2((float) uvBounds["right"] / atlasSize.x, (float) uvBounds["top"] / atlasSize.y)
        };
    }

    return result;
}

void UIBuilder::Build9Slice(DynamicVAO& vao, Position2D min, Position2D max, const NineSlice& nineSlice, const vec3& color)
{
    // c-----d
    // | g-h |
    // | e-f |
    // a-----b

    FixMinMax(min, max);

    Vertex a { min, vec2(0.f) },
           b { vec2(max.x, min.y), vec2(1.f, 0.f) },
           c { vec2(min.x, max.y), vec2(0.f, 1.f) },
           d { max, vec2(1.f) },
           e { min + nineSlice.CornerSize, vec2(nineSlice.Offset) },
           f { vec2(max.x - nineSlice.CornerSize, min.y + nineSlice.CornerSize), vec2(1.f - nineSlice.Offset, nineSlice.Offset) },
           g { vec2(min.x + nineSlice.CornerSize, max.y - nineSlice.CornerSize), vec2(nineSlice.Offset, 1.f - nineSlice.Offset) },
           h { max - nineSlice.CornerSize, vec2(1.f) - nineSlice.Offset };

    // Corners
    vao.PushQuad(a, e, color);
    vao.PushQuad(b, f, color);
    vao.PushQuad(c, g, color);
    vao.PushQuad(d, h, color);

    // Outter bits
    vao.PushQuad(e, Vertex{ vec2(f.Position.x, min.y), vec2(f.UV.x, 0.f)}, color);
    vao.PushQuad(e, Vertex{ vec2(min.x, g.Position.y), vec2(0.f, g.UV.y)}, color);

    vao.PushQuad(h, Vertex{ vec2(max.x, f.Position.y), vec2(1.f, f.UV.y)}, color);
    vao.PushQuad(h, Vertex{ vec2(g.Position.x, max.y), vec2( g.UV.x, 1.f)}, color);

    // Center
    vao.PushQuad(e, h, color);
}

void UIBuilder::BuildText(DynamicVAO& vao, Position2D position, float scale, std::string_view text, const Font& font, const vec3& color)
{
    Position2D offset = DEFAULT;

    for(const char character : text)
    {
        const Glyph& glyph = font.Glyphs[character];

        vao.PushQuad(glyph.Quad * scale + offset + position, color);
        offset.x += glyph.Advance * scale;
    }
}

void UIBuilder::BuildButton(Window& window, const CalculatorButton& button)
{
    Position2D min = (vec2) button.Position / (vec2) window.GetGridSize();
    Position2D max = (vec2) (button.Position + button.Size) / (vec2) window.GetGridSize();

    FixMinMax(min, max);

    Build9Slice(window.GetBackgroundVAO(), min, max, window.GetButtonNineSlice(), button.Color);

    Position2D textPosition = (min + max) / 2.f;
    const Position2D textSize = GetTextSize(TEXT_SCALE, button.Text, window.GetFont());

    //textPosition.y -= _font.LineHeight / 4.f * TEXT_SCALE;
    textPosition -= textSize / 2.f;

    BuildText(window.GetForegroundVAO(), textPosition, TEXT_SCALE, button.Text, window.GetFont());
}

Position2D UIBuilder::GetTextSize(float scale, std::string_view text, const Font& font)
{
    Position2D size = DEFAULT;

    for(const char character : text)
    {
        const Glyph& glyph = font.Glyphs[character];
        size.x += glyph.Advance * scale;
        size.y = std::max(size.y, glyph.Quad.Max.Position.y * scale);
    }

    return size;
}
