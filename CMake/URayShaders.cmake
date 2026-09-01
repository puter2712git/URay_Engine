# Compile each HLSL shader into the vertex and fragment SPIR-V modules expected
# by the renderer. DXC is installed with the Vulkan SDK on supported platforms.
find_program(DXC_EXECUTABLE
    NAMES dxc dxc.exe
    HINTS
        "$ENV{VULKAN_SDK}/Bin"
        "$ENV{VULKAN_SDK}/Bin32"
    REQUIRED
)

set(URAY_ENGINE_DIR "${URAY_SOURCE_DIR}/Engine")
set(URAY_SHADER_SOURCE_DIR
    "${URAY_ENGINE_DIR}/Asset/Source/Shader")
set(URAY_SHADER_OUTPUT_DIR
    "${URAY_ENGINE_DIR}/Asset/Imported/Shader")

file(GLOB_RECURSE URAY_HLSL_SHADERS CONFIGURE_DEPENDS
    "${URAY_SHADER_SOURCE_DIR}/*.hlsl")
file(GLOB_RECURSE URAY_HLSL_INCLUDES CONFIGURE_DEPENDS
    "${URAY_SHADER_SOURCE_DIR}/*.hlsli")

set(URAY_COMPILED_SHADERS)

foreach(shader_file IN LISTS URAY_HLSL_SHADERS)
    get_filename_component(shader_name "${shader_file}" NAME_WE)
    get_filename_component(shader_directory "${shader_file}" DIRECTORY)

    file(RELATIVE_PATH shader_relative_directory
        "${URAY_SHADER_SOURCE_DIR}"
        "${shader_directory}")

    set(shader_output_directory
        "${URAY_SHADER_OUTPUT_DIR}/${shader_relative_directory}")

    set(vertex_shader "${shader_output_directory}/${shader_name}.vert.spv")
    set(fragment_shader "${shader_output_directory}/${shader_name}.frag.spv")

    add_custom_command(
        OUTPUT "${vertex_shader}" "${fragment_shader}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${shader_output_directory}"
        COMMAND "${DXC_EXECUTABLE}"
            -spirv -T vs_6_0 -E VSMain -I "${URAY_SHADER_SOURCE_DIR}"
            "${shader_file}" -Fo "${vertex_shader}"
        COMMAND "${DXC_EXECUTABLE}"
            -spirv -T ps_6_0 -E PSMain -I "${URAY_SHADER_SOURCE_DIR}"
            "${shader_file}" -Fo "${fragment_shader}"
        DEPENDS "${shader_file}" ${URAY_HLSL_INCLUDES}
        COMMENT "Compiling ${shader_name}.hlsl to SPIR-V"
        VERBATIM
    )

    list(APPEND URAY_COMPILED_SHADERS "${vertex_shader}" "${fragment_shader}")
endforeach()

add_custom_target(URay_Shaders DEPENDS ${URAY_COMPILED_SHADERS})
add_dependencies(URay_Engine URay_Shaders)
