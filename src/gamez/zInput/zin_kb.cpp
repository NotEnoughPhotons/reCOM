#include "zinput.h"

#include <SDL3/SDL_keyboard.h>

std::map<SDL_Scancode, bool> CKeyboard::m_keys;

s32 InitKeyreadStuff()
{
    return SDL_HasKeyboard();
}

void CKeyboard::Init()
{
    // Primary movement keys
    m_keys.insert({ SDL_SCANCODE_W, false });
    m_keys.insert({ SDL_SCANCODE_A, false });
    m_keys.insert({ SDL_SCANCODE_S, false });
    m_keys.insert({ SDL_SCANCODE_D, false });

    // Jump key
    m_keys.insert({ SDL_SCANCODE_SPACE, false });
    m_keys.insert({ SDL_SCANCODE_LCTRL, false });

    // Running/sprinting
    m_keys.insert({ SDL_SCANCODE_LSHIFT, false });
}

void CKeyboard::Tick(f32 delta)
{

}