#pragma once

#include "Render/Shader/Reflection/ShaderReflection.h"

#include "Core/Type/Types.h"

#include <spirv/spirv_reflect.h>
#include <string>
#include <vector>

namespace URay::Render
{

class ShaderReflector
{
public:
    static bool Reflect(
        const std::vector<uint8>& shaderCode,
        const std::string& entryPoint,
        ShaderReflection& outReflection);

private:
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
