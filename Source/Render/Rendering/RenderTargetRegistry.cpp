#include "RenderTargetRegistry.h"

#include "Render/RHI/Device.h"

namespace URay::Render
{

RenderTargetRegistry::RenderTargetRegistry(Device& device) : device(device) {}

RenderTargetRegistry::~RenderTargetRegistry() = default;

bool RenderTargetRegistry::Create(
    RenderTargetId id,
    const RenderTargetDesc& desc,
    bool isViewportRelative)
{
    const size_t index = static_cast<size_t>(id);
    RenderTargetEntry& entry = entries[index];

    auto renderTarget = std::make_unique<RenderTarget>(device, desc);
    if (!renderTarget)
        return false;

    entry.desc = desc;
    entry.isViewportRelative = isViewportRelative;
    entry.target = std::move(renderTarget);

    return true;
}

void RenderTargetRegistry::Destroy(RenderTargetId id)
{
    const size_t index = static_cast<size_t>(id);
    RenderTargetEntry& entry = entries[index];

    if (!entry.target)
        return;

    entry.desc = {};
    entry.isViewportRelative = false;
    entry.target.reset();
}

void RenderTargetRegistry::DestroyAll()
{
    const size_t count = static_cast<size_t>(RenderTargetId::Count);

    for (size_t i = 0; i < count; ++i)
    {
        const RenderTargetId id = static_cast<RenderTargetId>(i);
        Destroy(id);
    }
}

RenderTarget& RenderTargetRegistry::Get(RenderTargetId id)
{
    const size_t index = static_cast<size_t>(id);
    RenderTargetEntry& entry = entries[index];

    return *entry.target;
}

const RenderTarget& RenderTargetRegistry::Get(RenderTargetId id) const
{
    const size_t index = static_cast<size_t>(id);
    const RenderTargetEntry& entry = entries[index];

    return *entry.target;
}

bool RenderTargetRegistry::RecreateViewportRelative(const Extent2D& extent)
{
    const size_t count = static_cast<size_t>(RenderTargetId::Count);

    for (size_t i = 0; i < count; ++i)
    {
        RenderTargetEntry& entry = entries[i];

        if (!entry.isViewportRelative)
            continue;

        entry.target->Recreate(extent);
    }

    return true;
}

} // namespace URay::Render
