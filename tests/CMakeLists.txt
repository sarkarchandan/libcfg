cmake_minimum_required(VERSION 3.0...3.22)

if(${CMAKE_VERSION} VERSION_LESS 3.22)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(cfg_test)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.0.1
)

FetchContent_MakeAvailable(Catch2)

set(SOURCES
    cfg_test_basic.cpp
)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} libcfg)
target_link_libraries(${PROJECT_NAME} Catch2::Catch2WithMain)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
catch_discover_tests(${PROJECT_NAME})