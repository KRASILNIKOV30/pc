add_executable(tv
        main.cpp
        src/TVReceiver/Client.h
        src/TVStation/Server.h
        src/pch.h
        src/TVStation/VideoCamera.h
        src/TVReceiver/ClientMode.h
        src/TVReceiver/TVReceiver.cpp
        src/TVReceiver/TVReceiver.h
        src/TVStation/TVStation.cpp
        src/TVStation/TVStation.h
        src/TVStation/ServerMode.h
)

set(BOOST_ROOT "/usr/include/boost")
find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenCV REQUIRED)

target_link_libraries(tv Boost::system)
target_link_libraries(tv ${OpenCV_LIBS})