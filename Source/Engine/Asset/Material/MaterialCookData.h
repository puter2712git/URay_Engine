#pragma once

#include "Core/File/VirtualPath.h"
#include "Core/Math/Color.h"

#include <string>

namespace URay
{

struct MaterialCookData
{
    std::string shaderName = "Mesh";
    Color baseColor = Color::White;
    VirtualPath baseColorTexturePath;
};

} // namespace URay
