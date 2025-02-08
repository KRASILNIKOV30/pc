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

target_link_libraries(make-archive PUBLIC timer)