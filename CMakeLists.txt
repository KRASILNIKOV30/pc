cmake_minimum_required(VERSION 3.26)
project(os)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

add_subdirectory(lab1)

add_library(catch2 INTERFACE)
add_library(timer INTERFACE)
add_library(coutBuffer INTERFACE)

target_include_directories(
        catch2 INTERFACE lib/catch2/
        timer INTERFACE lib/timer/
        coutBuffer INTERFACE lib/coutBuffer/
        osWrappers INTERFACE lib/osWrappers/
)