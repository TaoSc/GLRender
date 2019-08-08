#pragma once

#include <array>

#include <SDL.h>


class Input
{
public:
	Input();
	~Input();

	void updateEvents();
	static void showCursor(bool const& toggle);
	static void catchCursor(bool const& toggle);
	bool end() const;

	bool isKeyPressed(SDL_Scancode const& key) const;
	bool isKeyReleased(SDL_Scancode const& key) const;
	bool isMiceButtonPressed(Uint8 const& button) const;
	bool hasMiceMoved() const;
	bool hasWheelMoved() const;


	int getX() const;
	int getY() const;

	int getXRel() const;
	int getYRel() const;

	int getWheelX() const;
	int getWheelY() const;


private:
	SDL_Event m_events;
	std::array<bool, SDL_NUM_SCANCODES> m_keys;
	std::array<bool, SDL_NUM_SCANCODES> m_previous_keys;
	std::array<bool, 8> m_mice_buttons;


	int m_cursor_x;
	int m_cursor_y;

	int m_cursor_x_rel;
	int m_cursor_y_rel;

	int m_wheel_x;
	int m_wheel_y;
	SDL_bool m_wheel_moved;


	bool m_end;
};
