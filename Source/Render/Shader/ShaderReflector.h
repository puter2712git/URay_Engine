#pragma once

#include <vector>

namespace URay
{

class ShaderReflector
{
public:
    static bool ReflectSPIRV(const std::vector<uint8_t>& code);
};

} // namespace URay
