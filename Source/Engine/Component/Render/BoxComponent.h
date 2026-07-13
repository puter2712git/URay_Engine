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
    void SetVertexBuffer(VkBuffer buffer)
    {
        vertexBuffer = buffer;
    }

    VkBuffer GetIndexBuffer() const
    {
        return indexBuffer;
    }
    void SetIndexBuffer(VkBuffer buffer)
    {
        indexBuffer = buffer;
    }

private:
    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkBuffer indexBuffer = VK_NULL_HANDLE;
};

} // namespace URay
