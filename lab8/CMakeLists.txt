add_executable(particle-system
        glfwWindow/BaseWindow.h
        glfwWindow/BaseWindow.cpp
        glfwWindow/GLFWInitializer.h
        glfwWindow/GLFWInitializer.cpp
        window/Window.cpp
        window/Window.h
        main.cpp
)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(particle-system glfw OpenGL::GL)
