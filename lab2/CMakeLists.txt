add_executable(life
        life/main.cpp
        life/StepMode.h
        life/GenerateMode.h
        life/GenerateMode.cpp
        life/Life.h
        life/Life.cpp
)

add_executable(gauss
        gauss/main.cpp
        gauss/Gauss.h
        gauss/Gauss.cpp
        gauss/Pixels.h
        gauss/GaussianKernel.h
        gauss/Parallel.h
        gauss/Pixel.h
)

add_executable(gauss_view
        gauss/Gauss.h
        gauss/Gauss.cpp
        gauss/Pixels.h
        gauss/GaussianKernel.h
        gauss/Parallel.h
        gauss/Pixel.h
        gauss/ViewMain.cpp
)

find_package(wxWidgets REQUIRED COMPONENTS core base)
if (wxWidgets_USE_FILE)
    include(${wxWidgets_USE_FILE})
endif ()

target_link_libraries(gauss PRIVATE ${wxWidgets_LIBRARIES})
target_link_libraries(gauss_view PRIVATE ${wxWidgets_LIBRARIES})