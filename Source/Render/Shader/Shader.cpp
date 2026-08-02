#include "Shader.h"

namespace URay
{

Shader::Shader(uint32_t id,
               const ShaderStage& vertexStage,
               const ShaderStage& fragmentStage,
               const ShaderReflection& vertexReflection,
               const ShaderReflection& fragmentReflection)
    : id(id),
      vertexStage(vertexStage), fragmentStage(fragmentStage),
      vertexReflection(vertexReflection), fragmentReflection(fragmentReflection)
{
}

DescriptorSetLayoutDesc Shader::GetDescriptorSetLayoutDesc(uint32_t set) const
{
}

} // namespace URay
