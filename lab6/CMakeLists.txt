add_executable(tbb-example
        main.cpp
)

find_package(TBB REQUIRED)
target_link_libraries(tbb-example PRIVATE TBB::tbb)