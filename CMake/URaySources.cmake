file(GLOB_RECURSE URAY_SOURCES CONFIGURE_DEPENDS
    "${URAY_SOURCE_DIR}/Source/*.cpp"
    "${URAY_SOURCE_DIR}/ThirdParty/imgui/*.cpp"
)

file(GLOB_RECURSE URAY_HEADERS CONFIGURE_DEPENDS
    "${URAY_SOURCE_DIR}/Source/*.h"
)

target_sources(URay_Engine PRIVATE
    ${URAY_HEADERS}
    ${URAY_SOURCES}
    "${URAY_SOURCE_DIR}/ThirdParty/spirv/spirv_reflect.c"
)

target_include_directories(URay_Engine PRIVATE
    "${URAY_SOURCE_DIR}/Source"
    "${URAY_SOURCE_DIR}/ThirdParty"
)

target_compile_definitions(URay_Engine PRIVATE
    SPIRV_REFLECT_USE_SYSTEM_SPIRV_H
)