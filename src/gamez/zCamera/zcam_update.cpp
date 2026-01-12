#include "zcam.h"

#include "gamez/zVideo/zvid.h"
#include "gamez/zInput/zinput.h"
#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace zdb
{
	void CCamera::Update(tag_ZCAM_TYPE type)
	{
        m_forward = glm::vec3(0.0f, 0.0f, -1.0f);
        m_up = glm::vec3(0.0f, 1.0f, 0.0f);

        m_direction.x = cosf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch));
        m_direction.y = sinf(glm::radians(m_pitch));
        m_direction.z = sinf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch));

        m_forward = glm::normalize(m_direction);
        m_right = glm::normalize(glm::cross(m_forward, m_up));

        m_matrix = reinterpret_cast<CMatrix&>(glm::lookAt(m_position, m_position + m_forward, m_up));

        m_yaw += CMouse::m_mouseVelocity.x * CMouse::m_mouseSensitivity;
        m_pitch += CMouse::m_mouseVelocity.y * CMouse::m_mouseSensitivity;

        // HACK
        CMouse::m_mouseVelocity = glm::vec2(0.0f);

        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;

        f32 speed = CKeyboard::m_keys[SDL_SCANCODE_LSHIFT] ? 0.25f : 0.5f;

        if (CKeyboard::m_keys[SDL_SCANCODE_W])
            m_position += m_forward * speed;
        if (CKeyboard::m_keys[SDL_SCANCODE_S])
            m_position -= m_forward * speed;
        if (CKeyboard::m_keys[SDL_SCANCODE_D])
            m_position += m_right * speed;
        if (CKeyboard::m_keys[SDL_SCANCODE_A])
            m_position -= m_right * speed;
        if (CKeyboard::m_keys[SDL_SCANCODE_SPACE])
            m_position += m_up * speed;
        if (CKeyboard::m_keys[SDL_SCANCODE_LCTRL])
            m_position -= m_up * speed;
	}
}