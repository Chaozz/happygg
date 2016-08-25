include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(runBasicSrcTests
                basic_check.cpp search_test.cpp)

target_link_libraries(runBasicSrcTests gtest gtest_main)
target_link_libraries(runBasicSrcTests src)

add_test(NAME runBasicSrcTests
         COMMAND runBasicSrcTests)