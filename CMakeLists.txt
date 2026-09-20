cmake_minimum_required(VERSION 3.20)

project(URay_Engine)

set(URAY_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(URay_Engine)

include(CMake/URayOptions.cmake)
include(CMake/URaySources.cmake)
include(CMake/URayDependencies.cmake)

set(URAY_DOTNET_ROOT "C:/Program Files/dotnet" CACHE PATH
    ".NET installation root")

set(URAY_DOTNET_HOST_NATIVE_DIR
    "${URAY_DOTNET_ROOT}/packs/Microsoft.NETCore.App.Host.win-x64/10.0.12/runtimes/win-x64/native"
)

target_include_directories(URay_Engine PRIVATE
    "${URAY_DOTNET_HOST_NATIVE_DIR}"
)

target_link_libraries(URay_Engine PRIVATE
    "${URAY_DOTNET_HOST_NATIVE_DIR}/nethost.lib"
)

add_custom_command(TARGET URay_Engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${URAY_DOTNET_HOST_NATIVE_DIR}/nethost.dll"
        "$<TARGET_FILE_DIR:URay_Engine>/nethost.dll"
)