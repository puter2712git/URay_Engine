#include "Shader.h"

namespace URay
{

URAY_REGISTER_CLASS(Shader)

void Shader::RegisterClass()
{
    Super::RegisterClass();
}

Shader::Shader(const VirtualPath& filePath)
    : filePath(filePath) {}

Shader::~Shader() = default;

} // namespace URay
