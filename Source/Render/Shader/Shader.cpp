#include "Shader.h"

namespace URay
{

Shader::Shader(const ShaderStage& vertexStage, const ShaderStage& fragmentStage)
    : vertexStage(vertexStage), fragmentStage(fragmentStage)
{
}

} // namespace URay
