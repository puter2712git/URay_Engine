find_package(Vulkan REQUIRED)

add_subdirectory("${URAY_SOURCE_DIR}/Thirdparty/glfw")
add_subdirectory("${URAY_SOURCE_DIR}/Thirdparty/freetype-2.14.3")

include(FetchContent)

set(URAY_DXC_VERSION "v1.9.2607")
set(URAY_DXC_ARCHIVE_URL
    "https://github.com/microsoft/DirectXShaderCompiler/releases/download/${URAY_DXC_VERSION}/dxc_2026_07_29.zip"
)
set(URAY_DXC_ARCHIVE_SHA256
    "A1DFB116BA3EEAE6A1582291B53A8E7BF65AD760676BD3194685C8F7367CD241"
)

FetchContent_Declare(
    uray_dxc
    URL "${URAY_DXC_ARCHIVE_URL}"
    URL_HASH "SHA256=${URAY_DXC_ARCHIVE_SHA256}"
)

FetchContent_MakeAvailable(uray_dxc)

set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "Build yaml-cpp tests" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "Build yaml-cpp tools" FORCE)

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG yaml-cpp-0.9.0
)

FetchContent_MakeAvailable(yaml-cpp)

target_include_directories(URay_Engine SYSTEM PUBLIC
    "${uray_dxc_SOURCE_DIR}/inc"
)

target_link_libraries(URay_Engine PRIVATE
    URay_ThirdParty
    glfw
    Vulkan::Vulkan
    freetype
    yaml-cpp::yaml-cpp
    "${uray_dxc_SOURCE_DIR}/lib/x64/dxcompiler.lib"
)

target_link_libraries(URay_ThirdParty PRIVATE
    glfw
    Vulkan::Vulkan
)

add_custom_command(TARGET URay_Engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${uray_dxc_SOURCE_DIR}/bin/x64/dxcompiler.dll"
        "$<TARGET_FILE_DIR:URay_Engine>/dxcompiler.dll"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${uray_dxc_SOURCE_DIR}/bin/x64/dxil.dll"
        "$<TARGET_FILE_DIR:URay_Engine>/dxil.dll"
)
