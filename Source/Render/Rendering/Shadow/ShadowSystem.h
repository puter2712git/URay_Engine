#pragma once

#include "Core/Type/Types.h"

#include <memory>
#include <optional>
#include <vector>

namespace URay::Render
{

class RenderDevice;
class RenderTarget;
class SpotLightObject;

struct ShadowAtlasEntry
{
    uint32 x = 0;
    uint32 y = 0;
    uint32 width = 0;
    uint32 height = 0;
};

class ShadowSystem
{
public:
    ShadowSystem(RenderDevice& device);
    ~ShadowSystem();

public:
    bool Initialize();
    void Finalize();

    std::optional<ShadowAtlasEntry> GetOrAllocateEntry(SpotLightObject* object);
    void ReleaseEntry(SpotLightObject* object);
    void ClearEntries();

    RenderTarget* GetDirectionalTarget() const { return directionalRenderTarget.get(); }
    RenderTarget* GetShadowAtlasRT() const { return shadowAtlasRT.get(); }

private:
    RenderDevice& device;

    std::unique_ptr<RenderTarget> directionalRenderTarget = nullptr;
    std::unique_ptr<RenderTarget> shadowAtlasRT = nullptr;

    std::vector<SpotLightObject*> allocatedSpotLights;
};

} // namespace URay::Render
