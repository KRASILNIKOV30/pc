#include "glfwWindow/GLFWInitializer.h"
#include "window/Window.h"

int main()
{
	GLFWInitializer initGLFW;
	Window window{ 800, 600, "Particle system" };
	window.Run();
}