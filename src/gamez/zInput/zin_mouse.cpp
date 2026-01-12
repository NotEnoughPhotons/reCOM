#include "zinput.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_mouse.h"

f32 CMouse::m_mouseSensitivity = 1.0f;
glm::vec2 CMouse::m_mousePos;
glm::vec2 CMouse::m_mouseVelocity;

void CMouse::Tick(f32 dT)
{
	
}

void CMouse::ProcessMouseMotion(const SDL_MouseMotionEvent& event)
{
	SDL_Log("%f %f", event.x, event.y);
	m_mousePos = glm::vec2(event.x, event.y);
	m_mouseVelocity = glm::vec2(event.xrel, -event.yrel) * m_mouseSensitivity;
}
