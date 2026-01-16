add_library(chess_engine ./GameLogic.cpp ./MoveGenerator.cpp)
target_include_directories(chess_engine PUBLIC ${CMAKE_SOURCE_DIR}/include)