file(GLOB_RECURSE URAY_SOURCES CONFIGURE_DEPENDS
    "${URAY_SOURCE_DIR}/Source/*.cpp"
)

file(GLOB_RECURSE URAY_IMGUI_SOURCES CONFIGURE_DEPENDS
    "${URAY_SOURCE_DIR}/ThirdParty/imgui/*.cpp"
)

file(GLOB_RECURSE URAY_HEADERS CONFIGURE_DEPENDS
    "${URAY_SOURCE_DIR}/Source/*.h"
)

target_sources(URay_Engine PRIVATE
    ${URAY_HEADERS}
    ${URAY_SOURCES}
)

add_library(URay_ThirdParty STATIC)

target_sources(URay_ThirdParty PRIVATE
    ${URAY_IMGUI_SOURCES}
    "${URAY_SOURCE_DIR}/ThirdParty/spirv/spirv_reflect.c"
)

target_include_directories(URay_Engine PRIVATE
    "${URAY_SOURCE_DIR}/Source"
)

target_include_directories(URay_ThirdParty SYSTEM PUBLIC
    "${URAY_SOURCE_DIR}/ThirdParty"
)

# Suppress diagnostics from sources bundled with third-party dependencies while
# preserving the warning level configured for the engine's own source files.
if(MSVC)
    # Keep external-header diagnostics disabled when using CMake 3.20 or 3.21,
    # whose Ninja generator does not yet translate SYSTEM includes to /external:I.
    target_compile_options(URay_Engine PRIVATE
        "/external:I${URAY_SOURCE_DIR}/ThirdParty"
        /external:W0
    )

    target_compile_options(URay_ThirdParty PRIVATE
        "/external:I${URAY_SOURCE_DIR}/ThirdParty"
        /external:W0
        /utf-8
        /wd26439
        /wd26440
    )
endif()

target_compile_definitions(URay_Engine PRIVATE
    SPIRV_REFLECT_USE_SYSTEM_SPIRV_H
)

target_compile_definitions(URay_ThirdParty PRIVATE
    SPIRV_REFLECT_USE_SYSTEM_SPIRV_H
)
