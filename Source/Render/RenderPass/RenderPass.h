#pragma once

#include "Render/DrawCommand/DrawCommand.h"
#include "Render/RenderPass/RenderPassId.h"

#include "Core/Math/Matrix.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace URay::Render
{

class Renderer;

struct RenderPassContext
{
    Renderer& renderer;
};

class RenderPass
{
public:
    virtual ~RenderPass() = default;

public:
    virtual void Begin(const RenderPassContext&) = 0;
    virtual void End(const RenderPassContext&) = 0;

    virtual void Execute(const RenderPassContext&, const std::vector<DrawCommand>&) = 0;

    virtual RenderPassId GetPassId() const = 0;
};

} // namespace URay::Render
