/*
 * Camera.cpp
 *
 *  Created on: Jan 19, 2016
 *      Author: fabiano
 */

#include <string>
#include <iostream>

#include "Camera.h"

static const float KEYBOARD_SPEED = 0.1f;
static const float MOUSE_SPEED = 0.1f;

/*################################################################################*/
Camera::Camera(const glm::vec3 &position, float fov, float aspectRatio, float zNear, float zFar)
{
	m_perpective = glm::perspective(fov, aspectRatio, zNear, zFar);
	m_position = position;
	m_viewDirection = glm::vec3(0.0, 0.0, -1.0f);
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);
}

/*################################################################################*/
glm::mat4 Camera::GetWorldToViewMatrix() const
{
	return m_perpective * glm::lookAt(m_position, m_position + m_viewDirection, m_up);
}

/*################################################################################*/
void Camera::MouseUpdate(int mouseX, int mouseY)
{
	glm::vec2 newMousePosition = glm::vec2(mouseX-400, mouseY-300);
	glm::vec2 mouseDelta = newMousePosition - m_oldMousePosition;
	if (glm::length(mouseDelta) > 10.0f) {
		m_oldMousePosition = newMousePosition;
	}
	glm::vec3 toRotateAround = glm::cross(m_viewDirection, m_up);

	glm::mat4 rotator = glm::rotate(-mouseDelta.x*MOUSE_SPEED, m_up) *
						glm::rotate(-mouseDelta.y*MOUSE_SPEED, toRotateAround);

	m_viewDirection = glm::mat3(rotator) * m_viewDirection;

	m_oldMousePosition = newMousePosition;
}

/*################################################################################*/
void Camera::MoveForward()
{
	m_position += KEYBOARD_SPEED * m_viewDirection;
}

/*################################################################################*/
void Camera::MoveBackward()
{
	m_position += -KEYBOARD_SPEED * m_viewDirection;
}

/*################################################################################*/
void Camera::MoveLeft()
{
	glm::vec3 leftDirection = glm::cross(m_viewDirection, m_up);
	m_position += -KEYBOARD_SPEED * leftDirection;
}

/*################################################################################*/
void Camera::MoveRight()
{
	glm::vec3 rightDirection = glm::cross(m_viewDirection, m_up);
	m_position += KEYBOARD_SPEED * rightDirection;
}

/*################################################################################*/
void Camera::MoveUp()
{
	m_position += KEYBOARD_SPEED * m_up;
}

/*################################################################################*/
void Camera::MoveDown()
{
	m_position += -KEYBOARD_SPEED * m_up;
}

