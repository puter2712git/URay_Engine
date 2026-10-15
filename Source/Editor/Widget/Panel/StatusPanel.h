#pragma once

#include <Editor/Widget/Panel/Panel.h>

#include <array>

namespace URay
{

class Label;
class VerticalLayout;

class StatusPanel final : public Panel
{
    URAY_TYPE(StatusPanel, Panel)

public:
    StatusPanel();

public:
    void Arrange(const Rect& rect) override;

protected:
    void OnUpdate(float deltaTime) override;

private:
    static constexpr size_t MaxScopeRows = 5;
    static constexpr float ScopeRefreshInterval = 1.0f;

    void UpdateCpuFrameLabel();
    void UpdateScopeLabels();

private:
    VerticalLayout* layout = nullptr;

    Label* fpsLabel = nullptr;
    Label* cpuFrameLabel = nullptr;
    std::array<Label*, MaxScopeRows> scopeLabels = {};

    float scopeRefreshElapsed = ScopeRefreshInterval;
};

} // namespace URay
