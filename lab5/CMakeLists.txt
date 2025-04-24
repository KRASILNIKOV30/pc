add_executable(tv
        main.cpp
        src/TVReceiver/Client.h
        src/TVStation/Server.h
        src/pch.h
        src/TVStation/VideoCamera.h
        src/TVReceiver/ClientMode.h
        src/TVReceiver/TVReceiver.h
        src/TVStation/TVStation.h
        src/TVStation/ServerMode.h
        src/TVStation/Microphone.h
        src/TVReceiver/AudioPlayer.h
        src/PacketType.h
)

set(BOOST_ROOT "/usr/include/boost")
find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenCV REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(RTAUDIO REQUIRED rtaudio)
pkg_check_modules(OPUS REQUIRED opus)
include_directories(${OPUS_INCLUDE_DIRS})
include_directories(${RTAUDIO_INCLUDE_DIRS})

target_link_libraries(tv
        Boost::system
        ${OpenCV_LIBS}
        ${RTAUDIO_LIBRARIES}
        ${OPUS_LIBRARIES}
)