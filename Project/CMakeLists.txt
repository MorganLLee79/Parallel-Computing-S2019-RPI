cmake_minimum_required(VERSION 3.10)

project(Parallel-Project)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -g -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_executable(project.out src/project.cpp src/pthread-wrappers.cpp src/data-structures.cpp src/data-structures.h)

find_package(MPI REQUIRED)
find_package(Threads REQUIRED)
target_link_libraries(project.out Threads::Threads MPI::MPI_CXX -lzoltan)
