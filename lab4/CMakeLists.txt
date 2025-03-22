find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

add_executable(audio-player
        main.cpp
        lib/miniaudio.c
        lib/miniaudio.h
        src/ErrorCategory.h
        src/Device.h
        src/Player.h
        src/SineWaveGenerator.h
        src/Chord.h
        src/ChordsGenerator.h
        src/Parser.h)

set_property(TARGET audio-player PROPERTY CXX_STANDARD 20)

if (MSVC)
    target_compile_options(audio-player PRIVATE /W4 /WX)
else ()
    target_compile_options(audio-player PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif ()

target_link_libraries(audio-player sfml-graphics)
