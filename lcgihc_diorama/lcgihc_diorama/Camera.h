#pragma once

#include <glew.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>

#include <glfw3.h>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);

	void keyControl(bool* keys, GLfloat deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);

	glm::vec3 getCameraPosition();
	glm::vec3 getCameraDirection();
	glm::mat4 calculateViewMatrix();

	~Camera();

private:
	glm::vec3 position;		// vector de posición
	glm::vec3 front;		// vector de movimiento frontal
							// vector de dirección : position + front
	glm::vec3 up;			// vector de altura
	glm::vec3 right;		// vector de movimiento lateral -> presindible
	glm::vec3 worldUp;		// altura inicial

	GLfloat yaw;			// ángulo de giro a los lados
	GLfloat pitch;			// ángulo de giro hacia abajo/arriba

	GLfloat moveSpeed;		// velocidad de movimiento WASD
	GLfloat turnSpeed;		// sensibilidad del mouse

	bool viewType;			// Variable agregada por Aldo
	glm::vec3 floorPosition;		// Variable agregada por Aldo
	GLfloat floorYaw;
	GLfloat floorPitch;
	bool banderaCamara;

	void update();
};

