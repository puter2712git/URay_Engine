#pragma once

#include <Engine/Component/Component.h>

namespace URay
{

class LifecycleTestComponent final : public Component
{
    URAY_CLASS(LifecycleTestComponent, Component)

protected:
    void OnInitialize() override;
    void OnLoad() override;
    void OnUpdate(float deltaTime) override;
    void OnUnload() override;
    void OnFinalize() override;
};

} // namespace URay
