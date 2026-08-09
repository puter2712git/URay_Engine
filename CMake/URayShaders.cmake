# Compile each HLSL shader into the vertex and fragment SPIR-V modules expected
# by the renderer. DXC is installed with the Vulkan SDK on supported platforms.
find_program(DXC_EXECUTABLE
    NAMES dxc dxc.exe
    HINTS
        "$ENV{VULKAN_SDK}/Bin"
        "$ENV{VULKAN_SDK}/Bin32"
    REQUIRED
)

set(URAY_SHADER_DIR "${URAY_SOURCE_DIR}/Shader")
set(URAY_SHADER_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/Shader")

file(GLOB URAY_HLSL_SHADERS CONFIGURE_DEPENDS "${URAY_SHADER_DIR}/*.hlsl")
file(GLOB URAY_HLSL_INCLUDES CONFIGURE_DEPENDS "${URAY_SHADER_DIR}/*.hlsli")

set(URAY_COMPILED_SHADERS)
foreach(shader_file IN LISTS URAY_HLSL_SHADERS)
    get_filename_component(shader_name "${shader_file}" NAME_WE)

    set(vertex_shader "${URAY_SHADER_OUTPUT_DIR}/${shader_name}.vert.spv")
    set(fragment_shader "${URAY_SHADER_OUTPUT_DIR}/${shader_name}.frag.spv")

    add_custom_command(
        OUTPUT "${vertex_shader}" "${fragment_shader}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${URAY_SHADER_OUTPUT_DIR}"
        COMMAND "${DXC_EXECUTABLE}"
            -spirv -T vs_6_0 -E VSMain -I "${URAY_SHADER_DIR}"
            "${shader_file}" -Fo "${vertex_shader}"
        COMMAND "${DXC_EXECUTABLE}"
            -spirv -T ps_6_0 -E PSMain -I "${URAY_SHADER_DIR}"
            "${shader_file}" -Fo "${fragment_shader}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:URay_Engine>/Shader"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${vertex_shader}" "$<TARGET_FILE_DIR:URay_Engine>/Shader/${shader_name}.vert.spv"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${fragment_shader}" "$<TARGET_FILE_DIR:URay_Engine>/Shader/${shader_name}.frag.spv"
        DEPENDS "${shader_file}" ${URAY_HLSL_INCLUDES}
        COMMENT "Compiling ${shader_name}.hlsl to SPIR-V"
        VERBATIM
    )

    list(APPEND URAY_COMPILED_SHADERS "${vertex_shader}" "${fragment_shader}")
endforeach()

add_custom_target(URay_Shaders DEPENDS ${URAY_COMPILED_SHADERS})
add_dependencies(URay_Engine URay_Shaders)
