set(COMMON_SOURCES
        glfwWindow/BaseWindow.h
        glfwWindow/BaseWindow.cpp
        glfwWindow/GLFWInitializer.h
        glfwWindow/GLFWInitializer.cpp
        vector/Vector2.h
        vector/Vector3.h
        vector/Vector4.h
        vector/VectorMath.h
        matrix/Matrix3.h
        matrix/Matrix4.h
        Random.h
        gpuRunner/GPURunner.h
)

add_executable(particle-system
        ${COMMON_SOURCES}
        particleSystem/window/Window.cpp
        particleSystem/window/Window.h
        particleSystem/Model.h
        particleSystem/main.cpp
)

add_executable(gauss-gpu
        gpuRunner/GPURunner.h
        gauss/Gauss.h
        gauss/GaussianKernel.h
        gauss/ViewMain.cpp
)

add_executable(radial-gpu
        gpuRunner/GPURunner.h
        radial/Radial.h
        radial/ViewMain.cpp
)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenCL REQUIRED)

find_package(wxWidgets REQUIRED COMPONENTS core base)
if (wxWidgets_USE_FILE)
    include(${wxWidgets_USE_FILE})
endif ()

include_directories(${OpenCL_INCLUDE_DIRS})
target_link_libraries(particle-system
        glfw
        OpenGL::GL
        GLU
        ${OpenCL_LIBRARIES}
        ${GLUT_LIBRARIES}
)

target_link_libraries(gauss-gpu
        ${OpenCL_LIBRARIES}
        ${wxWidgets_LIBRARIES}
)

target_link_libraries(radial-gpu
        ${OpenCL_LIBRARIES}
        ${wxWidgets_LIBRARIES}
)
