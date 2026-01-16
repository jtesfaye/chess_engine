cmake_minimum_required(VERSION 3.29)
project(ChessEngine VERSION 1.0)
set(CMAKE_CXX_STANDARD 23)

find_package(GTest REQUIRED)
include(GoogleTest)
enable_testing()

add_subdirectory(src)

add_executable(main ./main.cpp)

target_link_libraries(main PRIVATE chess_engine)


add_subdirectory(test)