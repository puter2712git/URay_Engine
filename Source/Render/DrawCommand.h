#pragma once

#include "Core/Math/Matrix.h"

#include <vulkan/vulkan.h>

namespace URay
{

struct DrawCommand
{
    Matrix worldMatrix = Matrix::Identity;

    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkBuffer indexBuffer = VK_NULL_HANDLE;

    uint32_t indexCount = 0;
};

} // namespace URay
