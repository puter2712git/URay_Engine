cmake_minimum_required(VERSION 3.20)

project(URay_Engine)

set(URAY_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(URay_Engine)

include(CMake/URayOptions.cmake)
include(CMake/URaySources.cmake)
include(CMake/URayDependencies.cmake)
include(CMake/URayShaders.cmake)
