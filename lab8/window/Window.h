#pragma once
#include "../glfwWindow/BaseWindow.h"
#include "../Model.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glu.h>
#include <glm/ext.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

class Window : public BaseWindow
{
public:
	Window(int w, int h, const char* title);
	~Window() override
	{
		gluDeleteQuadric(m_sphereQuadric);
	}

private:
	void OnMouseButton(int button, int action, [[maybe_unused]] int mods) override;
	void OnMouseMove(double x, double y) override;
	void RotateCamera(double xAngleRadians, double yAngleRadians);
	void OnResize(int width, int height) override;
	void OnRunStart() override;
	void Draw(int width, int height) override;
	void SetupCameraMatrix();
	void ProcessInput();
	void CheckKeyPress(int key, std::function<void()> const& callback);
	void OnIdle(double deltaTime) override;
	void DrawSphere(const Vector3d& pos, double radius, const Vector4f& color);

	static constexpr double DISTANCE_TO_ORIGIN = 0.1;
	bool m_leftButtonPressed = false;
	glm::dvec2 m_mousePos = {};
	glm::dmat4x4 m_cameraMatrix = glm::lookAt(
		glm::dvec3{ 0.0, 0.0, DISTANCE_TO_ORIGIN },
		glm::dvec3{ 0.0, 0.0, 0.0 },
		glm::dvec3{ 0.0, 1.0, 0.0 });
	Particles m_particles;
	bool m_keyState[GLFW_KEY_LAST] = { false };
	GLUquadric* m_sphereQuadric;
	bool m_showPoints = true;
};