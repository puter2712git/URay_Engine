#include "Shader.h"

namespace URay
{

Shader::Shader(uint32_t id, const ShaderStage& vertexStage, const ShaderStage& fragmentStage)
    : id(id), vertexStage(vertexStage), fragmentStage(fragmentStage)
{
}

} // namespace URay
