#pragma once

#include <Core/Type/TypeAlias.h>
#include <Editor/Widget/Panel/Panel.h>

#include <array>

namespace URay
{

class Label;
class VerticalLayout;

class StatusPanel final : public Panel
{
    URAY_CLASS(StatusPanel, Panel)

public:
    StatusPanel();

public:
    void Arrange(const Rect& rect) override;

protected:
    void OnUpdate(float deltaTime) override;

private:
    static constexpr size_t MaxScopeRows = 5;
    static constexpr float ScopeRefreshInterval = 1.0f;

    void UpdateCullingStatsLabel();
    void UpdateCpuFrameLabel();
    void UpdateGpuFrameLabel();
    void UpdateCpuScopeLabels();
    void UpdateGpuScopeLabels();

private:
    VerticalLayout* layout = nullptr;

    Label* fpsLabel = nullptr;
    Label* cullingStatsLabel = nullptr;
    Label* cpuFrameLabel = nullptr;
    Label* gpuFrameLabel = nullptr;
    Array<Label*, MaxScopeRows> cpuScopeLabels = {};
    Array<Label*, MaxScopeRows> gpuScopeLabels = {};

    float scopeRefreshElapsed = ScopeRefreshInterval;
};

} // namespace URay
