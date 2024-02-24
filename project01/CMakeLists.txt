cmake_minimum_required(VERSION 3.10)
project(project01)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CPX_BASE "/opt/ibm/ILOG/CPLEX_Studio2211")
set(CPX_INCDIR "${CPX_BASE}/cplex/include")
set(CPX_LIBDIR "${CPX_BASE}/cplex/lib/x86-64_linux/static_pic/libcplex.a")

include_directories(${CPX_INCDIR})

add_executable(project01 
        src/main.cpp
        src/cpxmacro.h
        src/FileManager.cpp
        src/FileManager.h
        src/cpxmacro.h
)

target_link_libraries(project01 ${CPX_LIBDIR} pthread dl stdc++fs)