#pragma once

#include "Core/File/VirtualPath.h"
#include "Core/Math/Color.h"
#include "Core/UUID.h"

namespace URay
{

struct MaterialCookData
{
    UUID shaderUUID = {};
    Color baseColor = Color::White;
    VirtualPath baseColorTexturePath;
};

} // namespace URay
