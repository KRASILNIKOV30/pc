#pragma once

#include "../glfwWindow/BaseWindow.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext.hpp>

class Window : public BaseWindow {
public:
    Window(int w, int h, const char *title);

private:
    void OnMouseButton(int button, int action, [[maybe_unused]] int mods) override;

    void OnMouseMove(double x, double y) override;

    void RotateCamera(double xAngleRadians, double yAngleRadians);

    void OnResize(int width, int height) override;

    void OnRunStart() override;

    void Draw(int width, int height) override;

    void SetupCameraMatrix();

    static constexpr double DISTANCE_TO_ORIGIN = 2;
    bool m_leftButtonPressed = false;
    glm::dvec2 m_mousePos = {};
    glm::dmat4x4 m_cameraMatrix = glm::lookAt(
            glm::dvec3{0.0, 0.0, DISTANCE_TO_ORIGIN},
            glm::dvec3{0.0, 0.0, 0.0},
            glm::dvec3{0.0, 1.0, 0.0});
};