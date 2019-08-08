#pragma once

#include <map>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <SDL.h>

#include "Input.h"


class Camera
{
public:
	Camera();
	Camera(glm::vec3 position, glm::vec3 target, glm::vec3 world_up, float sensitivity = 0.85f, float speed = 0.25f);
	~Camera();

	void orientate(int const& rel_x, int const& rel_y, float const& delta_time);
	void shift(Input const& input, std::map<std::string, SDL_Scancode> const& keys, float const& delta_time);
	glm::mat4 lookAt() const;

	void setTarget();
	void setPosition(glm::vec3 const& position);

	float getSensitivity() const;
	float getSpeed() const;

	void setSensitivity(float const& sensitivity);
	void setSpeed(float const& speed);

	glm::vec3 getPosition() const;
	glm::vec3 getOrientation() const;


private:
	float m_yaw; // degrees
	float m_pitch; // degrees

	const glm::vec3 m_world_up;

	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_right;
	glm::vec3 m_up;

	float m_sensitivity;
	float m_speed;
};
