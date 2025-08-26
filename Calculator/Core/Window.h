//
// Created by scion on 6/7/2025.
//

#pragma once

#include <Calculator/Core/Graphics/Shader.h>
#include <Calculator/Core/Graphics/Texture.h>
#include <Calculator/Core/Graphics/VAO.h>
#include <Calculator/Core/Utility/Math.h>
#include <Calculator/Core/Utility/UIBuilder.h>
#include <Vendor/SDL3/SDL.h>

#include "Utility/Input.h"

enum class WindowFailCode : int
{
    SDLInitFailure = -2,
    SDLWindowInitFailure = -3,
    OpenGLInitFailure = -4,
    OpenGLLoaderFailure = -5,
};

void DebugMessageCallback(u32 source, u32 type, u32 id, u32 severity, i32 length, const char* message, const void* userData);

class Window
{
public:
    explicit Window(Size2D size);

    void Run();

    GET_CONST(Size2D, Size, _size);
    GET_CONST(Size2D, GridSize, _gridSize);
    GET_CONST(SDL_Window*, SDLWindow, _window);


    GET(std::string&, Equation, _equation);
    GET_SET(bool, UseSpecialButtons, _useSpecialButtons);

    GET_CONST(const UIBuilder::Font&, Font, _font);
    GET_CONST(const UIBuilder::NineSlice&, ButtonNineSlice, _buttonNineSlice);

    GET(DynamicVAO&, BackgroundVAO, _backgroundVAO);
    GET(DynamicVAO&, ForegroundVAO, _foregroundVAO);

private:
    void Init();
    void Render();
    void PollInputs();
    void Update();

    void CheckButton(CalculatorButton& button);

    SDL_Window* _window = nullptr;
    void* _graphicsContext = nullptr;
    bool _shouldClose = false;

    MouseState _mouse;

    Size2D _size = DEFAULT;
    bool _shouldRender = true;

    // UI stuff
    Size2D _gridSize;
    std::vector<CalculatorButton> _buttons;
    std::vector<CalculatorButton> _specialButtons;
    std::vector<CalculatorButton> _persistentButtons;
    std::string _equation;

    bool _useSpecialButtons = false;

    DynamicVAO _backgroundVAO, _foregroundVAO;
    Shader _uiShader;

    UIBuilder::NineSlice _buttonNineSlice;
    Texture _buttonTexture;
    UIBuilder::Font _font;
};
