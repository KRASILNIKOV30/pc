#include "glfwWindow/GLFWInitializer.h"
#include "window/Window.h"
#include <GL/glut.h>

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	GLFWInitializer initGLFW;
	Window window{ 800, 600, "Particle system" };
	window.Run();
}