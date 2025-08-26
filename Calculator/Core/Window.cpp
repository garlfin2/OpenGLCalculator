//
// Created by scion on 6/7/2025.
//

#include "Window.h"
#include "CalculatorFunctions.h"

#include <iostream>
#include <glad/glad.h>

#include "Graphics/VAO.h"
#include "Utility/UIBuilder.h"
#include <thread>
#include <chrono>

#define SDL_INIT_FLAGS SDL_INIT_VIDEO
#define SDL_WINDOW_FLAGS (SDL_WINDOW_OPENGL | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE | SDL_WINDOW_TRANSPARENT)

void DebugMessageCallback(u32 source, u32 type, u32 id, u32 severity, i32 length, const char* message, const void* userData)
{
    if(severity < GL_DEBUG_SEVERITY_HIGH)
        return;

    std::cout << message << std::endl;
}

Window::Window(Size2D size) :
    _mouse(this),
    _size(size)
{
    if(!SDL_WasInit(SDL_INIT_FLAGS))
        if(!SDL_Init(SDL_INIT_FLAGS))
            std::exit((int) WindowFailCode::SDLWindowInitFailure);

    _window = SDL_CreateWindow("Transparent Calculator", _size.x, _size.y, SDL_WINDOW_FLAGS);
    if(!_window)
        std::exit((int) WindowFailCode::SDLWindowInitFailure);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

#ifdef DEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

    _graphicsContext = SDL_GL_CreateContext(_window);
    if(!_graphicsContext)
        std::exit((int) WindowFailCode::OpenGLInitFailure);

    if(!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress))
        std::exit((int) WindowFailCode::OpenGLLoaderFailure);
}

void Window::Run()
{
    Init();

    while(!_shouldClose)
    {
        PollInputs();

        if(!_mouse.GetDidMove())
            std::this_thread::sleep_for(std::chrono::milliseconds(1000/30));

        Update();

        if(!_shouldRender)
            continue;
        _shouldRender = false;

        Render();

        SDL_GL_SwapWindow(_window);
    }
}

void Window::Init()
{
#ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(DebugMessageCallback, nullptr);
#endif

    glClearColor(0.f, 0.f, 0.f, 0.f);

    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    // Layers transparency
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_DST_ALPHA);

    _backgroundVAO = DynamicVAO(0);
    _foregroundVAO = DynamicVAO(0);
    _uiShader = Shader("ui.vert", "ui.frag");

    _buttonNineSlice = UIBuilder::NineSlice( 0.125f, 0.03f);
    _buttonTexture = PVR::FromFile("button.pvr");

    _font = UIBuilder::LoadFont("roboto.json", "roboto.pvr");

    _gridSize = Size2D(4, 7);

    // Bottom Row
    _buttons.push_back(ButtonFactory::CreateButton(Size2D(0), "0", NumberButtonClick, Size2D(2, 1)));
    _buttons.push_back(ButtonFactory::CreateButton(Size2D(2, 0), ".", DecimalButtonClick));
    _persistentButtons.push_back(ButtonFactory::CreateButton(Size2D(3, 0), "Enter", EvaluateButtonClick, Size2D(1, 2)));

    _buttons.push_back(ButtonFactory::CreateButton(Size2D(0, 1), "1", NumberButtonClick));
    _buttons.push_back(ButtonFactory::CreateButton(Size2D(1, 1), "2", NumberButtonClick));
    _buttons.push_back(ButtonFactory::CreateButton(Size2D(2, 1), "3", NumberButtonClick));

    _buttons.push_back(ButtonFactory::CreateButton(Size2D(0, 2), "4", NumberButtonClick));
    _buttons.push_back(ButtonFactory::CreateButton(Size2D(1, 2), "5", NumberButtonClick));
    _buttons.push_back(ButtonFactory::CreateButton(Size2D(2, 2), "6", NumberButtonClick));
    _buttons.push_back(ButtonFactory::CreateButton(Size2D(3, 2), "+", OperatorButtonClick));

    _buttons.push_back(ButtonFactory::CreateButton(Size2D(0, 3), "7", NumberButtonClick));
    _buttons.push_back(ButtonFactory::CreateButton(Size2D(1, 3), "8", NumberButtonClick));
    _buttons.push_back(ButtonFactory::CreateButton(Size2D(2, 3), "9", NumberButtonClick));
    _buttons.push_back(ButtonFactory::CreateButton(Size2D(3, 3), "-", OperatorButtonClick));

    _buttons.push_back(ButtonFactory::CreateButton(Size2D(0, 4), "-/+", NegateButtonClick));
    _buttons.push_back(ButtonFactory::CreateButton(Size2D(2, 4), "/", OperatorButtonClick));
    _buttons.push_back(ButtonFactory::CreateButton(Size2D(3, 4), "*", OperatorButtonClick));

    _persistentButtons.push_back(ButtonFactory::CreateButton(Size2D(3, 5), "AC", [](Window* window, const CalculatorButton&)
    {
        window->GetEquation().clear();
    }));
    _persistentButtons.push_back(ButtonFactory::CreateButton(Size2D(2, 5), "DEL", [](Window* window, const CalculatorButton&)
    {
        ClearExceptionState(window);
        window->_equation.resize(window->_equation.empty() ? 0 : window->_equation.size() - 1);
    }));
    _persistentButtons.push_back(ButtonFactory::CreateButton(Size2D(1, 4), "!%&...", [](Window* window, const CalculatorButton&)
    {
        window->_useSpecialButtons = !window->_useSpecialButtons;
    }));

    _persistentButtons.push_back(ButtonFactory::CreateButton(Size2D(0, 5), "(", NumberButtonClick));
    _persistentButtons.push_back(ButtonFactory::CreateButton(Size2D(1, 5), ")",  OperatorButtonClick));

    _specialButtons.push_back(ButtonFactory::CreateButton(Size2D(2, 4), "sin", TrigButtonClick));
    _specialButtons.push_back(ButtonFactory::CreateButton(Size2D(3, 4), "cos", TrigButtonClick));
    _specialButtons.push_back(ButtonFactory::CreateButton(Size2D(3, 3), "tan", TrigButtonClick));
    _specialButtons.push_back(ButtonFactory::CreateButton(Size2D(3, 2), "%",  [](Window* window, const CalculatorButton& button)
    {
        OperatorButtonClick(window, button);
        window->_useSpecialButtons = false;
    }));

    //_specialButtons.push_back(ButtonFactory::CreateButton(Size2D()))
}

void Window::PollInputs()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            _shouldClose = true;
            break;
        case SDL_EVENT_WINDOW_RESIZED:
            glViewport(0, 0, event.window.data1, event.window.data2);
            _size = Size2D(event.window.data1, event.window.data2);
            _shouldRender = true;
            break;
        default:
            break;
        }
    }

    _mouse.Update();
}

void Window::Update()
{
    for(CalculatorButton& button : _persistentButtons)
        CheckButton(button);

    std::vector<CalculatorButton>* buttons = &_buttons;
    if(_useSpecialButtons)
        buttons = &_specialButtons;

    for(CalculatorButton& button : *buttons)
        CheckButton(button);
}

void Window::CheckButton(CalculatorButton& button)
{
    const Size2D cellSize =  _size / _gridSize;
    const Size2D mousePosition = _mouse.GetPosition();

    Size2D min = _size / _gridSize * button.Position,
           max = min + cellSize * button.Size;

    const bool hovered = all(greaterThan(mousePosition, min)) && all(lessThan(mousePosition, max));
    const vec3 newColor = hovered ? vec3(0.5f) : vec3(1.f);

    if(hovered && _mouse.GetButton(MouseButton::Left) == KeyState::Pressed)
    {
        button.OnClick(this, button);
        _shouldRender = true;
    }

    if(button.Color != newColor)
        _shouldRender = true;

    button.Color = newColor;
}

void Window::Render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    _backgroundVAO.Clear();
    _foregroundVAO.Clear();

    for(const CalculatorButton& button : _persistentButtons)
        UIBuilder::BuildButton(*this, button);

    const std::vector<CalculatorButton>* buttons = &_buttons;
    if(_useSpecialButtons)
        buttons = &_specialButtons;

    for(const CalculatorButton& button : *buttons)
        UIBuilder::BuildButton(*this, button);

    UIBuilder::BuildButton(*this, CalculatorButton(Size2D(0, 6), DEFAULT, nullptr, Size2D(4, 1)));

    Position2D textPosition = Position2D(0.1f, 6.5f) / (Position2D) _gridSize;
    textPosition.y -= _font.LineHeight / 2.f * 0.05f;

    UIBuilder::BuildText(_foregroundVAO, textPosition, 0.05f, _equation, _font);

    _backgroundVAO.Update();
    _foregroundVAO.Update();

    _uiShader.Bind();

    _uiShader.SetUniform(0, _buttonTexture, 0);
    _uiShader.SetUniform(1, false);
    _backgroundVAO.Draw();

    _uiShader.SetUniform(0, _font.Texture, 0);
    _uiShader.SetUniform(1, true);
    _foregroundVAO.Draw();
}


