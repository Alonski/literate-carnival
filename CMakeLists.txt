cmake_minimum_required(VERSION 3.10)
project(literate_carnival)

set(CMAKE_CXX_STANDARD 11)

add_executable(literate_carnival
        .idea/codeStyles/Project.xml
        .idea/literate-carnival.iml
        .idea/misc.xml
        .idea/modules.xml
        .idea/vcs.xml
        .idea/workspace.xml
        cmake-build-debug/cmake_install.cmake
        cmake-build-debug/CMakeCache.txt
        cmake-build-debug/Makefile
        cmake-build-debug/Project.cbp
        .gitignore
        CMakeLists.txt
        LICENSE
        README.md
        sudoku_threads.c
        sudoku_threads_mutex.c
        threads
        sudoku_threads.h)
