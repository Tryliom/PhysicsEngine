cmake_minimum_required(VERSION 3.22)
project(PhysicsEngine)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

OPTION(ENABLE_SANITIZERS "Enable sanitizers" OFF)
OPTION(USE_TRACY "Enable Tracy profiling" OFF)

IF(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2 /Oi /Oy-")

    IF (ENABLE_SANITIZERS)
        # Disable edit and continue
        string(REPLACE "/ZI" "/Zi" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
        string(REPLACE "/ZI" "/Zi" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
        string(REPLACE "/ZI" "/Zi" CMAKE_CXX_FLAGS_MINSIZEREL ${CMAKE_CXX_FLAGS_MINSIZEREL})
        # Disable /RTC1
        string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
        string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
        string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_MINSIZEREL ${CMAKE_CXX_FLAGS_MINSIZEREL})
        # Disable /INCREMENTAL
        string(REPLACE "/INCREMENTAL" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
        string(REPLACE "/INCREMENTAL" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
        string(REPLACE "/INCREMENTAL" "" CMAKE_CXX_FLAGS_MINSIZEREL ${CMAKE_CXX_FLAGS_MINSIZEREL})

        add_compile_options(/fsanitize=address /fno-omit-frame-pointer)
    ENDIF()
else()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -save-temps -march=haswell -ffast-math")
ENDIF()

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
endforeach()

# Physics library
file(GLOB_RECURSE SRC_FILES physics/include/*.h physics/src/*.cpp)
add_library(PhysicsEngineLib ${SRC_FILES})
set_target_properties(PhysicsEngineLib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(PhysicsEngineLib PUBLIC physics/include/)
target_include_directories(PhysicsEngineLib PUBLIC libs/Math/)
target_link_libraries(PhysicsEngineLib PUBLIC CommonLib)

# Physics tests
SET(PHYSICS_TEST_DIR ${CMAKE_SOURCE_DIR}/physics/tests)
file(GLOB PHYSICS_TEST_FILES ${PHYSICS_TEST_DIR}/*.cpp )

foreach(test_file ${PHYSICS_TEST_FILES} )
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(${test_name} ${test_file})

    target_link_libraries(${test_name} PUBLIC PhysicsEngineLib)
    target_link_libraries(${test_name} PRIVATE GTest::gtest GTest::gtest_main)
endforeach()

# Graphics library
SET(GRAPHICS_DIR ${CMAKE_SOURCE_DIR}/graphics)
file(GLOB_RECURSE GRAPHICS_FILES ${GRAPHICS_DIR}/src/*.cpp ${GRAPHICS_DIR}/include/*.h)

add_library(GraphicsLib ${GRAPHICS_FILES})
target_include_directories(GraphicsLib PUBLIC ${GRAPHICS_DIR}/include/)
target_include_directories(GraphicsLib PUBLIC physics/include/)
target_include_directories(GraphicsLib PUBLIC libs/Math/)
target_link_libraries(GraphicsLib PUBLIC PhysicsEngineLib SDL2::SDL2 CommonLib imgui::imgui)

# Samples
SET(SAMPLE_DIR ${CMAKE_SOURCE_DIR}/samples)
file(GLOB_RECURSE Samples ${SAMPLE_DIR}/include/*.h ${SAMPLE_DIR}/src/*.cpp)

add_executable(Samples ${SAMPLE_DIR}/main.cpp)
add_library(SamplesLib ${Samples})

target_include_directories(SamplesLib PUBLIC ${SAMPLE_DIR}/include/)
target_include_directories(SamplesLib PUBLIC ${GRAPHICS_DIR}/include/)
target_include_directories(SamplesLib PUBLIC physics/include/)
target_include_directories(SamplesLib PUBLIC libs/Math/)
target_link_libraries(SamplesLib PUBLIC PhysicsEngineLib SDL2::SDL2 GraphicsLib CommonLib imgui::imgui)

target_link_libraries(Samples PUBLIC SamplesLib)

if (USE_TRACY)
    add_subdirectory(libs/tracy-0.10) # target: TracyClient or alias Tracy::TracyClient
    # Link Tracy::TracyClient to any target where you use Tracy for profiling:
    target_link_libraries(PhysicsEngineLib PUBLIC Tracy::TracyClient)
    target_link_libraries(CommonLib PUBLIC Tracy::TracyClient)
    target_link_libraries(GraphicsLib PUBLIC Tracy::TracyClient)
    target_link_libraries(SamplesLib PUBLIC Tracy::TracyClient)
endif()
