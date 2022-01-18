cmake_minimum_required(VERSION 3.16)
project(Ex5)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_COMPILER /usr/bin/g++ )


add_executable(Ex5
        source.cpp
        IRsource.cpp
        SymbolsRepo.cpp
        Type.cpp
        hw3_output.cpp
        RegGenerator.cpp
        bp.cpp
        )