cmake_minimum_required(VERSION 3.22)
project(math)

set(CMAKE_CXX_STANDARD 17)

find_package(GTest CONFIG REQUIRED)

file(GLOB_RECURSE SRC_FILES include/*.h)
add_library(Common ${SRC_FILES})
set_target_properties(Common PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(Common PUBLIC include/)

# Tests
SET(TEST_DIR ${CMAKE_SOURCE_DIR}/tests)
file(GLOB TEST_FILES ${TEST_DIR}/*.cpp)

foreach(test_file ${TEST_FILES} )
    # I used a simple string replace, to cut off .cpp.
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(${test_name} ${test_file})
    # Make sure YourLib is linked to each app
    target_link_libraries(${test_name} PUBLIC Common)
    target_link_libraries(${test_name} PRIVATE GTest::gtest GTest::gtest_main)

    IF(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2 /Oi /Oy-")
    else()
        set_target_properties(${test_name} PROPERTIES COMPILE_FLAGS "-save-temps -march=haswell -ffast-math -fno-omit-frame-pointer")
    ENDIF()

    set_target_properties (${test_name} PROPERTIES FOLDER Courses)
endforeach()
