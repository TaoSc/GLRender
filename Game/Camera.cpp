#include "Camera.h"

#include <cmath>

#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>

#include "Input.h"
#include <iostream>


Camera::Camera() : m_yaw(0), m_pitch(0), m_front(0.0f, 0.0f, -1.0f), m_world_up(0, 0, 1), m_right(0.f), m_position(0.f), m_sensitivity(0), m_speed(0)
{

}

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 world_up, float sensitivity, float speed) :
	m_yaw(0), m_pitch(0), m_front(0.0f, 0.0f, -1.0f), m_right(0.f), m_world_up(world_up), m_position(position), m_sensitivity(sensitivity), m_speed(speed)
{
	setTarget();
}

Camera::~Camera()
{
}


void Camera::orientate(int const& rel_x, int const& rel_y, float const& delta_time)
{
	const float sensitivity = m_sensitivity * delta_time;
	m_pitch += -rel_y * sensitivity;
	m_yaw += -rel_x * sensitivity;

	if (m_pitch > 89.0)
		m_pitch = 89.0;
	else if (m_pitch < -89.0)
		m_pitch = -89.0;

	float radian_pitch = glm::radians(m_pitch);
	float radian_yaw = glm::radians(m_yaw);


	if (m_world_up.x == 1.0)
	{
		m_front.x = sin(radian_pitch);
		m_front.y = cos(radian_pitch) * cos(radian_yaw);
		m_front.z = cos(radian_pitch) * sin(radian_yaw);
	}
	else if (m_world_up.y == 1.0)
	{
		m_front.x = cos(radian_pitch) * sin(radian_yaw);
		m_front.y = sin(radian_pitch);
		m_front.z = cos(radian_pitch) * cos(radian_yaw);
	}
	else
	{
		m_front.x = cos(radian_pitch) * cos(radian_yaw);
		m_front.y = cos(radian_pitch) * sin(radian_yaw);
		m_front.z = sin(radian_pitch);
	}
}

void Camera::shift(Input const& input, std::map<std::string, SDL_Scancode> const& keys, float const& delta_time)
{
	if (input.hasMiceMoved())
		orientate(input.getXRel(), input.getYRel(), delta_time);

	const float previous_y_position = m_position.y;
	const float speed = m_speed * delta_time;

	if (input.isKeyPressed(keys.at("forward")))
		m_position += m_front * speed;
	if (input.isKeyPressed(keys.at("backward")))
		m_position -= m_front * speed;

	m_right = glm::normalize(glm::cross(m_front, m_world_up));

	if (input.isKeyPressed(keys.at("left")))
		m_position -= m_right * speed;
	if (input.isKeyPressed(keys.at("right")))
		m_position += m_right * speed;

	m_position.y = previous_y_position;
	m_up = glm::normalize(glm::cross(m_right, m_front));

	if (input.isKeyPressed(keys.at("crouch")))
		m_position -= m_world_up * speed;
	if (input.isKeyPressed(keys.at("jump")))
		m_position += m_world_up * speed;
}

glm::mat4 Camera::lookAt() const
{
	return glm::lookAt(m_position, m_position + m_front, m_world_up);
}


void Camera::setTarget()
{
	m_front = glm::normalize(m_front);


	if (m_world_up.x == 1.0)
	{
		m_pitch = asin(m_front.x);
		m_yaw = acos(m_front.y / cos(m_pitch));

		if (m_front.y < 0)
			m_yaw *= -1;
	}
	else if (m_world_up.y == 1.0)
	{
		m_pitch = asin(m_front.y);
		m_yaw = acos(m_front.z / cos(m_pitch));

		if (m_front.z < 0)
			m_yaw *= -1;
	}
	else
	{
		m_pitch = asin(m_front.x);
		m_yaw = acos(m_front.z / cos(m_pitch));

		if (m_front.z < 0)
			m_yaw *= -1;
	}


	m_pitch = glm::degrees(m_pitch);
	m_yaw = glm::degrees(m_yaw);
}


void Camera::setPosition(glm::vec3 const& position)
{
	m_position = position;
}


float Camera::getSensitivity() const
{
	return m_sensitivity;
}

float Camera::getSpeed() const
{
	return m_speed;
}


void Camera::setSensitivity(float const& sensitivity)
{
	m_sensitivity = sensitivity;
}

void Camera::setSpeed(float const& speed)
{
	m_speed = speed;
}


glm::vec3 Camera::getPosition() const
{
	return m_position;
}

glm::vec3 Camera::getOrientation() const
{
	return m_front;
}
