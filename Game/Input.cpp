#include "Input.h"

#include <array>

#include <SDL.h>


Input::Input() : m_events(), m_cursor_x(0), m_cursor_y(0), m_cursor_x_rel(0), m_cursor_y_rel(0), m_wheel_x(0), m_wheel_y(0), m_wheel_moved(SDL_FALSE), m_end(false)
{
	m_keys.fill(false);
	m_previous_keys.fill(false);
	m_mice_buttons.fill(false);
}

Input::~Input()
{
}


void Input::updateEvents()
{
	m_previous_keys = m_keys;
	m_cursor_x_rel = 0;
	m_cursor_y_rel = 0;
	m_wheel_moved = SDL_FALSE;

	while (SDL_PollEvent(&m_events))
	{
		switch (m_events.type)
		{
		case SDL_KEYDOWN:
			m_keys[m_events.key.keysym.scancode] = true;
			break;

		case SDL_KEYUP:
			m_keys[m_events.key.keysym.scancode] = false;
			break;


		case SDL_MOUSEBUTTONDOWN:
			m_mice_buttons[m_events.button.button] = true;
			break;

		case SDL_MOUSEBUTTONUP:
			m_mice_buttons[m_events.button.button] = false;
			break;

		case SDL_MOUSEMOTION:
			m_cursor_x = m_events.motion.x;
			m_cursor_y = m_events.motion.y;

			m_cursor_x_rel = m_events.motion.xrel;
			m_cursor_y_rel = m_events.motion.yrel;
			break;

		case SDL_MOUSEWHEEL:
			m_wheel_x = m_events.wheel.x;
			m_wheel_y = m_events.wheel.y;

			if (m_events.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
			{
				m_wheel_x *= -1;
				m_wheel_y *= -1;
			}

			m_wheel_moved = SDL_TRUE;
			break;


		case SDL_WINDOWEVENT:
			if (m_events.window.event == SDL_WINDOWEVENT_CLOSE)
				m_end = true;
			break;


		default:
			break;
		}
	}
}

void Input::showCursor(bool const& toggle)
{
	SDL_ShowCursor(toggle ? SDL_ENABLE : SDL_DISABLE);
}

void Input::catchCursor(bool const& toggle)
{
	SDL_SetRelativeMouseMode(toggle ? SDL_TRUE : SDL_FALSE);
}

bool Input::end() const
{
	return m_end;
}


bool Input::isKeyPressed(SDL_Scancode const& key) const
{
	return m_keys[key];
}

bool Input::isKeyReleased(SDL_Scancode const& key) const
{
	return !m_keys[key] && m_previous_keys[key];
}

bool Input::isMiceButtonPressed(Uint8 const& button) const
{
	return m_mice_buttons[button];
}

bool Input::hasMiceMoved() const
{
	return !(m_cursor_x_rel == 0 && m_cursor_y_rel == 0);
}

bool Input::hasWheelMoved() const
{
	return m_wheel_moved == SDL_TRUE;
}


int Input::getX() const
{
	return m_cursor_x;
}

int Input::getY() const
{
	return m_cursor_y;
}

int Input::getXRel() const
{
	return m_cursor_x_rel;
}

int Input::getYRel() const
{
	return m_cursor_y_rel;
}

int Input::getWheelX() const
{
	return m_wheel_x;
}

int Input::getWheelY() const
{
	return m_wheel_y;
}
