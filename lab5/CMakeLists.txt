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
        src/TVStation/Microphone.h
)

set(BOOST_ROOT "/usr/include/boost")
find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenCV REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(RTAUDIO REQUIRED rtaudio)
include_directories(${RTAUDIO_INCLUDE_DIRS})

target_link_libraries(tv
        Boost::system
        ${OpenCV_LIBS}
        ${RTAUDIO_LIBRARIES}
)