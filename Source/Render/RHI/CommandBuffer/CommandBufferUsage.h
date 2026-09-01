#pragma once

#include "Core/Type/Types.h"

#include <vulkan/vulkan.h>
#include <cstdint>

namespace URay::Render
{

enum class CommandBufferUsage : uint32
{
    None = 0,
    OneTimeSubmit = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    RenderPassContinue = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
    SimultaneousUse = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
};

}
