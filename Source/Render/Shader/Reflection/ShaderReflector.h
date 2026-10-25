#pragma once
#include <Core/Type/Types.h>

#include <Render/Shader/Reflection/ShaderReflection.h>


#include <spirv/spirv_reflect.h>
#include <vector>

namespace URay::Render
{

class ShaderReflector
{
public:
    static bool Reflect(
        const std::vector<uint8>& shaderCode,
        const String& entryPoint,
        ShaderReflection& outReflection);

private:
    static bool ReflectVertexInputs(
        const SpvReflectShaderModule& module,
        const String& entryPoint,
        ShaderReflection& outReflection);

    static bool ReflectDescriptorBindings(
        const SpvReflectShaderModule& module,
        ShaderReflection& outReflection);

    static bool ReflectUniformBuffers(
        const SpvReflectShaderModule& module,
        ShaderReflection& outReflection);

    static bool ReflectPushConstants(
        const SpvReflectShaderModule& module,
        ShaderReflection& outReflection);
};

} // namespace URay::Render
