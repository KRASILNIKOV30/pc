set(CMAKE_CXX_COMPILER g++-12)

add_executable(my-task_tests
        myTask/MyTask.h
        tests/MyTask-tests.cpp
)

add_executable(generator
        generator/Generator.h
        generator/main.cpp
)

target_link_libraries(my-task_tests catch2)