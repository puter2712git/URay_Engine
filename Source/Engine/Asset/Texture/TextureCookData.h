#pragma once

#include "Core/Type/Types.h"

#include <yaml-cpp/yaml.h>

#include <vector>

namespace URay
{

struct TextureCookData
{
    int32 width = 0;
    int32 height = 0;
    int32 channels = 0;
    std::vector<uint8> pixels;
};

} // namespace URay
