#include "Shader.h"

namespace URay
{

Shader::Shader(const std::string& filePath, const std::vector<char>& code,
               VkShaderStageFlagBits stage, const std::string& entry)
    : filePath(filePath), code(code), stage(stage), entry(entry)
{
}

} // namespace URay
