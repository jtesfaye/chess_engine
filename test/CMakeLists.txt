function(add_gtest test_name)
  add_executable(${test_name} ${ARGN})
  target_link_libraries(${test_name} PRIVATE GTest::gtest GTest::gtest_main chess_engine)
  target_include_directories(${test_name} PUBLIC ${CMAKE_SOURCE_DIR}/include)
  include(GoogleTest)
  gtest_discover_tests(${test_name})
endfunction()

add_gtest(test_GameLogic ./test_GameLogic.cpp)