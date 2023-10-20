cmake_minimum_required(VERSION 3.22)
project(PhysicsEngine)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)
find_package(GTest CONFIG REQUIRED)

# Common library
file(GLOB_RECURSE COMMON_FILES common/include/*.h common/src/*.cpp)
add_library(CommonLib ${COMMON_FILES})
set_target_properties(CommonLib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(CommonLib PUBLIC common/include/)

# Common tests
SET(COMMON_TEST_DIR ${CMAKE_SOURCE_DIR}/common/tests)
file(GLOB COMMON_TEST_FILES ${COMMON_TEST_DIR}/*.cpp )

foreach(test_file ${COMMON_TEST_FILES} )
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(${test_name} ${test_file})

    target_link_libraries(${test_name} PUBLIC CommonLib)
    target_link_libraries(${test_name} PRIVATE GTest::gtest GTest::gtest_main)

    IF(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2 /Oi /Oy-")
    else()
        set_target_properties(${test_name} PROPERTIES COMPILE_FLAGS "-save-temps -march=haswell -ffast-math -fno-omit-frame-pointer")
    ENDIF()
endforeach()

# Physics library
file(GLOB_RECURSE SRC_FILES physics/include/*.h physics/src/*.cpp)
add_library(PhysicsEngineLib ${SRC_FILES})
set_target_properties(PhysicsEngineLib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(PhysicsEngineLib PUBLIC physics/include/)
target_include_directories(PhysicsEngineLib PUBLIC libs/Math/)

# Physics tests
SET(PHYSICS_TEST_DIR ${CMAKE_SOURCE_DIR}/physics/tests)
file(GLOB PHYSICS_TEST_FILES ${PHYSICS_TEST_DIR}/*.cpp )

foreach(test_file ${PHYSICS_TEST_FILES} )
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

# Graphics library
SET(GRAPHICS_DIR ${CMAKE_SOURCE_DIR}/graphics)
file(GLOB_RECURSE GRAPHICS_FILES ${GRAPHICS_DIR}/src/*.cpp ${GRAPHICS_DIR}/include/*.h)

add_library(GraphicsLib ${GRAPHICS_FILES})
target_include_directories(GraphicsLib PUBLIC ${GRAPHICS_DIR}/include/)
target_include_directories(GraphicsLib PUBLIC physics/include/)
target_include_directories(GraphicsLib PUBLIC libs/Math/)
target_link_libraries(GraphicsLib PUBLIC PhysicsEngineLib SDL2::SDL2 CommonLib)

# Samples
SET(SAMPLE_DIR ${CMAKE_SOURCE_DIR}/samples)
file(GLOB SAMPLES ${SAMPLE_DIR}/*)

foreach(sample ${SAMPLES})
    get_filename_component(sample_name ${sample} NAME_WE)
    file(GLOB_RECURSE ${sample_name}_FILES ${sample}/src/*.cpp ${sample}/include/*.h)
    list(LENGTH ${sample_name}_FILES num_files)

    add_executable(Sample_${sample_name} ${sample}/main.cpp)

    if (num_files GREATER 0)
        add_library(${sample_name}_LIB ${${sample_name}_FILES})
        target_include_directories(${sample_name}_LIB PUBLIC ${sample}/include/)
    else ()
        add_library(${sample_name}_LIB ${sample}/main.cpp)
    endif ()

    target_include_directories(${sample_name}_LIB PUBLIC ${GRAPHICS_DIR}/include/)
    target_include_directories(${sample_name}_LIB PUBLIC physics/include/)
    target_include_directories(${sample_name}_LIB PUBLIC libs/Math/)
    target_include_directories(${sample_name}_LIB PUBLIC common/include/)
    target_link_libraries(${sample_name}_LIB PUBLIC PhysicsEngineLib SDL2::SDL2 GraphicsLib CommonLib)

    target_link_libraries(Sample_${sample_name} PUBLIC ${sample_name}_LIB)

    IF(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2 /Oi /Oy-")
    else()
        set_target_properties(Sample_${sample_name} PROPERTIES COMPILE_FLAGS "-save-temps -march=haswell -ffast-math -fno-omit-frame-pointer")
    ENDIF()
endforeach()