add_executable(audio_player
        main.cpp
        lib/miniaudio.c
        lib/miniaudio.h
        src/ErrorCategory.h
        src/Device.h
        src/Player.h
        src/SineWaveGenerator.h
        src/Chord.h
        src/ChordsGenerator.h)

set_property(TARGET audio_player PROPERTY CXX_STANDARD 20)

if (MSVC)
    target_compile_options(audio_player PRIVATE /W4 /WX)
else ()
    target_compile_options(audio_player PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif ()
