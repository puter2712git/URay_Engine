#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

namespace URay::Render
{

enum class CommandPoolFlags : uint32_t
{
    None = 0,
    Transient = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
    ResetCommandBuffer = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
};

}
