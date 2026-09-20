#pragma once

#include "Core/Type/Types.h"

#include <memory>
#include <optional>
#include <vector>

namespace URay::Render
{

class RenderDevice;
class RenderTarget;
class Texture;
class TextureView;
class SpotLightObject;
class PointLightObject;

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

    std::optional<uint32> GetOrAllocatePointShadowIndex(PointLightObject* object);
    void ReleasePointLightEntry(PointLightObject* object);
    void ClearPointLightEntries();

    RenderTarget* GetDirectionalTarget() const { return directionalRenderTarget.get(); }
    RenderTarget* GetShadowAtlasRT() const { return shadowAtlasRT.get(); }
    Texture* GetDepthTextureCubeArray() const { return depthTextureCubeArray.get(); }
    TextureView* GetPointShadowSamplingView() const { return pointShadowSamplingView.get(); }
    TextureView* GetPointShadowFaceView(uint32 lightIndex, uint32 faceIndex) const { return pointShadowFaceViews[lightIndex * 6 + faceIndex].get(); }

private:
    RenderDevice& device;

    std::unique_ptr<RenderTarget> directionalRenderTarget = nullptr;
    std::unique_ptr<RenderTarget> shadowAtlasRT = nullptr;

    std::unique_ptr<Texture> depthTextureCubeArray = nullptr;
    std::unique_ptr<TextureView> pointShadowSamplingView = nullptr;
    std::vector<std::unique_ptr<TextureView>> pointShadowFaceViews;

    std::vector<SpotLightObject*> allocatedSpotLights;
    std::vector<PointLightObject*> allocatedPointLights;
};

} // namespace URay::Render
