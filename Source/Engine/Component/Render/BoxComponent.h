#pragma once

#include "Engine/Component/Render/RenderComponent.h"

#include <vulkan/vulkan.h>

namespace URay
{

class BoxComponent : public RenderComponent
{
public:
    BoxComponent() = default;
    ~BoxComponent() = default;

public:
    virtual void SubmitCommand() override;

    VkBuffer GetVertexBuffer() const
    {
        return vertexBuffer;
    };
    VkDeviceMemory GetVertexBufferMemory() const
    {
        return vertexBufferMemory;
    }

private:
    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
};

} // namespace URay
