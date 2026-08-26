find_package(Vulkan REQUIRED)

add_subdirectory("${URAY_SOURCE_DIR}/thirdparty/glfw")
add_subdirectory("${URAY_SOURCE_DIR}/thirdparty/freetype-2.14.3")

include(FetchContent)

set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "Build yaml-cpp tests" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "Build yaml-cpp tools" FORCE)

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG yaml-cpp-0.9.0
)

FetchContent_MakeAvailable(yaml-cpp)

target_link_libraries(URay_Engine PRIVATE
    URay_ThirdParty
    glfw
    Vulkan::Vulkan
    freetype
    yaml-cpp::yaml-cpp
)

target_link_libraries(URay_ThirdParty PRIVATE
    glfw
    Vulkan::Vulkan
)
