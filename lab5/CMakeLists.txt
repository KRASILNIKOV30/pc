add_executable(tv
        main.cpp
        src/Client.h
        src/Server.h
        src/pch.h
)

set(BOOST_ROOT "/usr/include/boost")
find_package(Boost REQUIRED COMPONENTS system)

target_link_libraries(tv Boost::system)