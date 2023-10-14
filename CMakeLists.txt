cmake_minimum_required(VERSION 3.22)
project(PhysicsEngine)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)
find_package(GTest CONFIG REQUIRED)

file(GLOB_RECURSE SRC_FILES common/include/*.h common/src/*.cpp)
add_library(PhysicsEngineLib ${SRC_FILES})
set_target_properties(PhysicsEngineLib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(PhysicsEngineLib PUBLIC common/include/)
target_include_directories(PhysicsEngineLib PUBLIC libs/Math/include/)
target_link_libraries(PhysicsEngineLib PRIVATE SDL2::SDL2)

# Make that the include direction in libs/Math/ is directly accessible
add_subdirectory(libs/Math)

# Disable SDL main
add_definitions(-DSDL_MAIN_HANDLED)

# Tests
SET(TEST_DIR ${CMAKE_SOURCE_DIR}/physics_tests)
file(GLOB TEST_FILES ${TEST_DIR}/*.cpp )

foreach(test_file ${TEST_FILES} )
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(${test_name} ${test_file})

    target_link_libraries(${test_name} PUBLIC PhysicsEngineLib)
    target_link_libraries(${test_name} PRIVATE GTest::gtest GTest::gtest_main)

    IF(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2 /Oi /Oy-")
    else()
        set_target_properties(${test_name} PROPERTIES COMPILE_FLAGS "-save-temps -march=haswell -ffast-math -fno-omit-frame-pointer")
    ENDIF()
endforeach()

# Samples
SET(SAMPLE_DIR ${CMAKE_SOURCE_DIR}/samples)
file(GLOB SAMPLES ${SAMPLE_DIR}/*)

foreach(sample ${SAMPLES})
    get_filename_component(sample_name ${sample} NAME_WE)
    file(GLOB_RECURSE ${sample_name}_FILES ${sample}/src/*.cpp ${sample}/include/*.h)

    if (EXISTS ${sample_name}_FILES)
        add_library(${sample_name}_LIB ${sample_name}_FILES)
    endif ()

    add_executable(${sample_name} ${sample}/main.cpp)

    target_include_directories(${sample_name} PUBLIC ${sample}/include/)
    target_link_libraries(${sample_name} PUBLIC PhysicsEngineLib SDL2::SDL2)

    if (EXISTS ${sample_name}_LIB)
        target_link_libraries(${sample_name} PUBLIC ${sample_name}_LIB)
    endif ()

    IF(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2 /Oi /Oy-")
    else()
        set_target_properties(${sample_name} PROPERTIES COMPILE_FLAGS "-save-temps -march=haswell -ffast-math -fno-omit-frame-pointer")
    ENDIF()
endforeach()