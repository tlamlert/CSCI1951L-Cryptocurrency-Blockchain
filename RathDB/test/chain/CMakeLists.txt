cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SRCS *.cpp)

include(GoogleTest)

add_executable(test_chain ${SRCS})
target_link_libraries(test_chain rathDB ${GTest_LIBRARIES})

gtest_add_tests(TARGET test_chain TEST_PREFIX "")