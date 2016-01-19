/*
 * Camera.h
 *
 *  Created on: Jan 18, 2016
 *      Author: fabiano
 */

#ifndef CAMERA_H_
#define CAMERA_H_
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Camera {
	public:
		Camera(const glm::vec3 &position, float fov, float aspectRatio, float zNear, float zFar);
		virtual ~Camera() {};

		glm::mat4 GetWorldToViewMatrix() const;

		void MouseUpdate(int mouseX, int mouseY);

		void MoveForward();
		void MoveBackward();
		void MoveLeft();
		void MoveRight();
		void MoveUp();
		void MoveDown();

	private:

		glm::mat4 m_perpective;
		glm::vec3 m_position;
		glm::vec3 m_viewDirection;
		glm::vec3 m_up;
		glm::vec2 m_oldMousePosition;
};

#endif /* CAMERA_H_ */
