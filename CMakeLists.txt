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
)

file(GLOB_RECURSE PROJECT_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

add_executable(URay_Engine
    ${PROJECT_SOURCES}
)

find_package(Vulkan REQUIRED)

add_subdirectory(ThirdParty/glfw)

target_include_directories(URay_Engine
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(URay_Engine
    PRIVATE
    glfw
    Vulkan::Vulkan
)

add_custom_command(TARGET URay_Engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Shader
        "$<TARGET_FILE_DIR:URay_Engine>/Shader"
)

set_target_properties(URay_Engine PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:URay_Engine>"
)
