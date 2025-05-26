cmake_minimum_required(VERSION 3.26)
project(pc)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_subdirectory(lab1)
add_subdirectory(lab2)
add_subdirectory(lab3)
add_subdirectory(lab4)
add_subdirectory(lab5)
add_subdirectory(lab6)
add_subdirectory(lab7)
add_subdirectory(lab8)

add_library(catch2 INTERFACE)
add_library(timer INTERFACE)
add_library(coutBuffer INTERFACE)

target_include_directories(
        catch2 INTERFACE lib/catch2/
        timer INTERFACE lib/timer/
        coutBuffer INTERFACE lib/coutBuffer/
        osWrappers INTERFACE lib/osWrappers/
)