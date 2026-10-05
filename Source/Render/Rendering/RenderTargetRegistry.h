#pragma once

#include "Render/RHI/RenderTarget.h"

#include "Core/Type/Types.h"

#include <array>

namespace URay::Render
{

class Device;

enum class RenderTargetId : uint8
{
    Scene,
    PostProcess,
    SelectionMask,
    GizmoDepth,
    Count
};

struct RenderTargetEntry
{
    RenderTargetDesc desc = {};
    bool isViewportRelative = false;
    std::unique_ptr<RenderTarget> target = nullptr;
};

class RenderTargetRegistry
{
public:
    RenderTargetRegistry(Device& device);
    ~RenderTargetRegistry();

public:
    bool Create(RenderTargetId id, const RenderTargetDesc& desc, bool isViewportRelative);

    void Destroy(RenderTargetId id);
    void DestroyAll();

    RenderTarget& Get(RenderTargetId id);
    const RenderTarget& Get(RenderTargetId id) const;

    bool RecreateViewportRelative(const Extent2D& extent);

private:
    Device& device;

    std::array<RenderTargetEntry, static_cast<size_t>(RenderTargetId::Count)> entries;
};

} // namespace URay::Render
