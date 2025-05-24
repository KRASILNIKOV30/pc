set(CMAKE_CXX_COMPILER g++-12)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

add_executable(my-task_tests
        myTask/MyTask.h
        tests/MyTask-tests.cpp
)

add_executable(generator
        generator/Generator.h
        generator/main.cpp
)

add_executable(generator_tests
        generator/Generator.h
        tests/Generator-tests.cpp
)

add_executable(my-awaiter
        simpleAwaiter/main.cpp
)

add_executable(async-file_tests
        asyncFile/Dispatcher.h
        asyncFile/AsyncFile.h
        asyncFile/Task.h
        asyncFile/OpenAsyncFile.h
        asyncFile/CopyFile.h
        tests/AsyncFile-tests.cpp
)

add_executable(async-file
        asyncFile/Dispatcher.h
        asyncFile/main.cpp
        asyncFile/AsyncFile.h
        asyncFile/OpenAsyncFile.h
        asyncFile/Task.h
        asyncFile/CopyFile.h
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBURING REQUIRED liburing)

target_link_libraries(my-task_tests catch2)
target_link_libraries(generator_tests catch2)

target_link_libraries(async-file_tests PRIVATE
        ${LIBURING_LIBRARIES}
        catch2
)
target_include_directories(async-file_tests PRIVATE ${LIBURING_INCLUDE_DIRS})

target_link_libraries(async-file PRIVATE ${LIBURING_LIBRARIES})
target_include_directories(async-file PRIVATE ${LIBURING_INCLUDE_DIRS})