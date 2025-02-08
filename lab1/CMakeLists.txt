add_executable(make-archive
        makeArchive/main.cpp
        ChunkVector.h
        Timer.h
        Gzip.h
        WaitChildProcesses.h
)

add_executable(extract-files
        extractFiles/main.cpp
        ChunkVector.h
        Timer.h
        Gzip.h
        WaitChildProcesses.h
)

include(FetchContent)
FetchContent_Declare(GSL
        GIT_REPOSITORY "https://github.com/microsoft/GSL"
        GIT_TAG "v4.1.0"
        GIT_SHALLOW ON
)
FetchContent_MakeAvailable(GSL)
target_link_libraries(make-archive PRIVATE Microsoft.GSL::GSL)
target_link_libraries(extract-files PRIVATE Microsoft.GSL::GSL)