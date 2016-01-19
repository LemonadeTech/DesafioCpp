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
		Camera(const glm::vec3 &position, float fov, float aspectRatio, float zNear, float zFar)
		{
			m_perpective = glm::perspective(fov, aspectRatio, zNear, zFar);
			m_position = position;
			m_forward = glm::vec3(0.0, 0.0 ,1.0);
			m_up = glm::vec3(0.0, 1.0, 0.0);
		}

		virtual ~Camera() {}

		inline glm::mat4 GetViewProjection() const
		{
			return m_perpective * glm::lookAt(m_position, m_position+m_forward, m_up);
		}

		inline void SetZoomIn(void)
		{
			m_position.z -= 0.2f;
		}

		inline void SetZoomOut(void)
		{
			m_position.z += 0.2f;
		}


	private:

		glm::mat4 m_perpective;
		glm::vec3 m_position;
		glm::vec3 m_forward;
		glm::vec3 m_up;
};

#endif /* CAMERA_H_ */
