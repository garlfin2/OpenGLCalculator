//
// Created by scion on 6/12/2025.
//

#include "Input.h"

#include <SDL3/SDL_mouse.h>
#include <Calculator/Core/Window.h>

void MouseState::Update()
{
    float x, y;

    SDL_SetWindowRelativeMouseMode(_window->GetSDLWindow(), false);
    SDL_ShowCursor();
    const u32 state = SDL_GetMouseState(&x, &y);

    const vec2 oldPosition = _mousePosition;
    _mousePosition = vec2(x, _window->GetSize().y - y);

    _moved = oldPosition != _mousePosition;

    for(u8 i = 0; i < 5; i++)
        UpdateKeyState(state >> i & 1, _buttons[i]);
}
