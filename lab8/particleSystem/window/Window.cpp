#include "Window.h"
#include <cmath>
#include <GL/gl.h>
#include <GL/glut.h>

namespace
{
constexpr double FIELD_OF_VIEW = 60 * M_PI / 180.0;
constexpr int PARTICLES_NUMBER = 3'000;
constexpr double Z_NEAR = 1.0;
constexpr double Z_FAR = 400;

// Ортонормируем матрицу 4*4 (это должна быть аффинная матрица)
glm::dmat4x4 Orthonormalize(const glm::dmat4x4& m)
{
	// Извлекаем подматрицу 3*3 из матрицы m и ортонормируем её
	const auto normalizedMatrix = glm::orthonormalize(glm::dmat3x3{ m });
	// Заменяем 3 столбца исходной матрицы
	return {
		glm::dvec4{ normalizedMatrix[0], 0.0 },
		glm::dvec4{ normalizedMatrix[1], 0.0 },
		glm::dvec4{ normalizedMatrix[2], 0.0 },
		m[3]
	};
}

} // namespace

Window::Window(int w, int h, const char* title)
	: BaseWindow(w, h, title)
{
}

void Window::OnMouseButton(int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_1)
	{
		m_leftButtonPressed = (action & GLFW_PRESS) != 0;
	}
	if (button == GLFW_MOUSE_BUTTON_2 && (action & GLFW_PRESS) != 0)
	{
		m_showPoints = !m_showPoints;
	}
}

void Window::OnMouseMove(double x, double y)
{
	const glm::dvec2 mousePos{ x, y };
	if (m_leftButtonPressed)
	{
		const auto windowSize = GetFramebufferSize();

		const auto mouseDelta = mousePos - m_mousePos;
		const double xAngle = mouseDelta.y * M_PI / windowSize.y;
		const double yAngle = mouseDelta.x * M_PI / windowSize.x;
		RotateCamera(xAngle, yAngle);
	}
	m_mousePos = mousePos;
}

// Вращаем камеру вокруг начала координат
void Window::RotateCamera(double xAngleRadians, double yAngleRadians)
{
	const glm::dvec3 xAxis{
		m_cameraMatrix[0][0], m_cameraMatrix[1][0], m_cameraMatrix[2][0]
	};
	const glm::dvec3 yAxis{
		m_cameraMatrix[0][1], m_cameraMatrix[1][1], m_cameraMatrix[2][1]
	};
	m_cameraMatrix = glm::rotate(m_cameraMatrix, xAngleRadians, xAxis);
	m_cameraMatrix = glm::rotate(m_cameraMatrix, yAngleRadians, yAxis);

	m_cameraMatrix = Orthonormalize(m_cameraMatrix);
}

void Window::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	// Вычисляем соотношение сторон клиентской области окна
	double aspect = double(width) / double(height);

	glMatrixMode(GL_PROJECTION);
	const auto proj = glm::perspective(FIELD_OF_VIEW, aspect, Z_NEAR, Z_FAR);
	glLoadMatrixd(&proj[0][0]);
	glMatrixMode(GL_MODELVIEW);
}

void Window::OnRunStart()
{
	for (int i = 0; i < PARTICLES_NUMBER; ++i)
	{
		m_particles.Add();
	}

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat lightPos[] = { 0.0f, 0.0f, 100.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glEnable(GL_DEPTH_TEST);

	m_sphereQuadric = gluNewQuadric();
	gluQuadricNormals(m_sphereQuadric, GLU_SMOOTH);
}

void Window::Draw(int width, int height)
{
	glClearColor(0.05, 0.05, 0.2, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ProcessInput();
	SetupCameraMatrix();

	if (m_showPoints)
	{
		glPointSize(3.0f);
		glDisable(GL_LIGHTING);
		glBegin(GL_POINTS);
		m_particles.ForEach([&](const ParticleView& p) {
			glColor3f(p.color.x, p.color.y, p.color.z);
			glVertex3d(p.pos.x, p.pos.y, p.pos.z);
		});
		glEnd();
	}
	else
	{
		glEnable(GL_LIGHTING);
		m_particles.ForEach([&](const ParticleView& p) {
			double radius = 0.2 * std::cbrt(p.pos.w);
			DrawSphere({ p.pos.x, p.pos.y, p.pos.z }, radius, p.color);
		});
	}

	DrawStats();
}

void Window::DrawSphere(const Vector3f& pos, double radius, const Vector4f& color)
{
	glPushMatrix();
	glTranslated(pos.x, pos.y, pos.z);

	// Материал сферы
	GLfloat matAmbient[] = { color.x * 0.3f, color.y * 0.3f, color.z * 0.3f, color.w };
	GLfloat matDiffuse[] = { color.x, color.y, color.z, color.w };
	GLfloat matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);

	gluSphere(m_sphereQuadric, radius, 16, 16);

	glPopMatrix();
}

void Window::SetupCameraMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(&m_cameraMatrix[0][0]);
}

void Window::ProcessInput()
{
	CheckKeyPress(GLFW_KEY_1, [&] {
		m_particles.Add();
	});
	CheckKeyPress(GLFW_KEY_2, [&] {
		m_particles.Delete();
	});
	CheckKeyPress(GLFW_KEY_UP, [&] {
		m_particles.IncreaseGravity();
	});
	CheckKeyPress(GLFW_KEY_DOWN, [&] {
		m_particles.DecreaseGravity();
	});
	CheckKeyPress(GLFW_KEY_LEFT, [&] {
		m_particles.SlowDownTime();
	});
	CheckKeyPress(GLFW_KEY_RIGHT, [&] {
		m_particles.SpeedUpTime();
	});
}
void Window::CheckKeyPress(int key, const std::function<void()>& callback)
{
	const auto keyPressed = IsKeyPressed(key);
	if (keyPressed && !m_keyState[key])
	{
		callback();
		m_keyState[key] = true;
	}
	else if (!keyPressed)
	{
		m_keyState[key] = false;
	}
}
void Window::OnIdle(double deltaTime)
{
	m_particles.Update(deltaTime);

	m_accumulatedTime += deltaTime;
	++m_frameCount;

	if (m_accumulatedTime >= 1.0)
	{
		m_fps = static_cast<double>(m_frameCount) / m_accumulatedTime;
		m_frameCount = 0;
		m_accumulatedTime = 0.0;
	}
}

void Window::DrawStats()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	const auto size = GetFramebufferSize();
	gluOrtho2D(0, size.x, size.y, 0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glColor3f(1.0f, 1.0f, 1.0f);
	const auto g = "G: " + std::to_string(m_particles.GetGravityConstant());
	const auto time = "TimeCoef: " + std::to_string(m_particles.GetTime());
	const auto numParticles = "NumParticles: " + std::to_string(m_particles.GetNumParticles());
	const auto fps = "FPS: " + std::to_string(m_fps);

	DrawText(g, 20.0, 30.0);
	DrawText(time, 20.0, 50.0);
	DrawText(numParticles, 20.0, 70.0);
	DrawText(fps, 20.0, 90.0);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	if (!m_showPoints)
	{
		glEnable(GL_LIGHTING);
	}
}

void Window::DrawText(std::string const& text, GLfloat x, GLfloat y)
{
	glRasterPos2f(x, y);
	for (char c : text)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
}
