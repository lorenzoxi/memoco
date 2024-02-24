cmake_minimum_required(VERSION 3.10)
project(project02)
set(CMAKE_CXX_STANDARD 17)

add_executable(project02 src/main.cpp
        include/Individual.h
        src/Individual.cpp
        src/Generation.cpp
        include/Generation.h
        src/Generation.cpp
        include/schemes/SelectionScheme.h
        include/Life.h
        include/schemes/TerminationScheme.h
        src/Life.cpp
        src/Board.cpp
        include/Board.h
        src/utils/FileManager.cpp
        include/utils/FileManager.h
        include/schemes/MutationScheme.h
        include/schemes/InitScheme.h
)