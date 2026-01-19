add_library(chess_engine ./ChessGame.cpp ./MoveGenerator.cpp ./GameTypes.cpp)
target_include_directories(chess_engine PUBLIC ${CMAKE_SOURCE_DIR}/include)