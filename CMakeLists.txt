cmake_minimum_required(VERSION 2.8)
project(happygg)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES happygg.cpp)

add_executable(happygg ${SOURCE_FILES})

include_directories(src)

add_subdirectory(src)
target_link_libraries(happygg src)

###############
### Testing ###
###############

enable_testing()
add_subdirectory(src_tests)