add_executable(particle-system
        glfwWindow/BaseWindow.h
        glfwWindow/BaseWindow.cpp
        glfwWindow/GLFWInitializer.h
        glfwWindow/GLFWInitializer.cpp
        particleSystem/window/Window.cpp
        particleSystem/window/Window.h
        vector/Vector2.h
        vector/Vector3.h
        vector/Vector4.h
        vector/VectorMath.h
        matrix/Matrix3.h
        matrix/Matrix4.h
        particleSystem/Model.h
        Random.h
        particleSystem/main.cpp
        gpuRunner/GPURunner.h
)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenCL REQUIRED)

include_directories(${OpenCL_INCLUDE_DIRS})
target_link_libraries(particle-system
        glfw
        OpenGL::GL
        GLU
        ${OpenCL_LIBRARIES}
        ${GLUT_LIBRARIES}
)
