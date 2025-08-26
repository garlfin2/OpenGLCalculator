//
// Created by scion on 6/12/2025.
//

#pragma once

#include "Math.h"

enum class KeyState : u8
{
    Up = 0,
    Down = 1 << 0,
    StateChanged = 1 << 1,
    Pressed = Down | StateChanged,
    Released = Up | StateChanged,
    None = 0
};

void UpdateKeyState(bool pressed, KeyState& key);

inline KeyState operator~(KeyState e) { return (KeyState) ~(u8) e; }
inline KeyState operator&(KeyState a, KeyState b) { return (KeyState) ((u8) a & (u8) b); }
inline KeyState operator|(KeyState a, KeyState b) { return (KeyState) ((u8) a | (u8) b); }
inline KeyState operator^(KeyState a, KeyState b) { return (KeyState) ((u8) a ^ (u8) b); }
inline KeyState operator<<(KeyState a, KeyState b) { return (KeyState) ((u8) a << (u8) b); }

inline bool IsKeyDown(KeyState state) { return (bool) (state & KeyState::Down); }

enum class MouseButton : u8
{
    Left = 0,
    Middle = 1,
    Right = 2,
    Front = 3,
    Back = 4,
    Size
};

class Window;

class MouseState
{
public:
    explicit MouseState(Window* window) : _window(window) {};

    GET_CONST(vec2, Position, _mousePosition);
    GET_CONST(bool, DidMove, _moved);

    NODISCARD KeyState GetButton(MouseButton button) const { return _buttons[(u8) button]; }

    void Update();

private:
    Window* _window;

    vec2 _mousePosition = DEFAULT;
    bool _moved = false;
    KeyState _buttons[(u8) MouseButton::Size] = DEFAULT;
};

inline void UpdateKeyState(bool pressed, KeyState& key)
{
    const KeyState previousState = key;

    key = pressed ? KeyState::Down : KeyState::Up;
    key = key | ((key ^ previousState) & KeyState::Down) << (KeyState) 1;
}