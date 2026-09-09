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
    static bool ReflectSPIRV(
        const std::vector<uint8>& code,
        const std::string& entryPoint,
        ShaderReflection& outReflection);

private:
    static bool ReflectDescriptorBindings(
        const SpvReflectShaderModule& module,
        const char* entryPoint,
        ShaderReflection& outReflection);

    static bool ReflectPushConstantBlocks(
        const SpvReflectShaderModule& module,
        const char* entryPoint,
        ShaderReflection& outReflection);

    static ReflectedBlockMember ReflectBlockMember(
        const SpvReflectBlockVariable& variable);
};

} // namespace URay::Render
