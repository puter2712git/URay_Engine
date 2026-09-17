#pragma once

#include <memory>

namespace URay::Render
{

class RenderDevice;
class RenderTarget;

class ShadowSystem
{
public:
    ShadowSystem(RenderDevice& device);
    ~ShadowSystem();

public:
    bool Initialize();
    void Finalize();

    RenderTarget* GetDirectionalTarget() const { return directionalRenderTarget.get(); }

private:
    RenderDevice& device;

    std::unique_ptr<RenderTarget> directionalRenderTarget = nullptr;
};

} // namespace URay::Render
