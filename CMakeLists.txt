cmake_minimum_required(VERSION 3.20)

project(URay_Engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

file(GLOB_RECURSE PROJECT_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/*.cpp"
)

file(GLOB_RECURSE PROJECT_HEADERS
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

add_executable(URay_Engine
        ${PROJECT_HEADERS}
        ${PROJECT_SOURCES}
        ThirdParty/spirv/spirv_reflect.c
)

target_compile_definitions(URay_Engine
        PRIVATE
        SPIRV_REFLECT_USE_SYSTEM_SPIRV_H
)

find_package(Vulkan REQUIRED)

add_subdirectory(ThirdParty/glfw)
add_subdirectory(ThirdParty/freetype-2.14.3)

target_include_directories(URay_Engine
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty
)

target_link_libraries(URay_Engine
        PRIVATE
        glfw
        Vulkan::Vulkan
        freetype
)

include(FetchContent)

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG master # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)
FetchContent_MakeAvailable(yaml-cpp)

target_link_libraries(URay_Engine PUBLIC yaml-cpp::yaml-cpp) # The library or executable that require yaml-cpp library

add_custom_command(TARGET URay_Engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Asset
        "$<TARGET_FILE_DIR:URay_Engine>/Asset"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Shader
        "$<TARGET_FILE_DIR:URay_Engine>/Shader"
)

set_target_properties(URay_Engine PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:URay_Engine>"
)
