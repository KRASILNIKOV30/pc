add_executable(audio_player
        "audio_player.cpp"
        "miniaudio.c"
        "miniaudio.h")

set_property(TARGET audio_player PROPERTY CXX_STANDARD 20)

if (MSVC)
    target_compile_options(audio_player PRIVATE /W4 /WX)
else ()
    target_compile_options(audio_player PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif ()
