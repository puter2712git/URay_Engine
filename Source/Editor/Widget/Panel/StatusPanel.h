#pragma once

#include "Editor/Widget/Panel/Panel.h"

namespace URay
{

class Label;
class Button;

class StatusPanel final : public Panel
{
    URAY_TYPE(StatusPanel, Panel)

public:
    StatusPanel();
    ~StatusPanel() override;

public:
    void Arrange(const Rect& rect) override;

protected:
    void OnUpdate(float deltaTime) override;

private:
    Label* statusLabel = nullptr;
    Label* fpsLabel = nullptr;
    Button* button = nullptr;
};

} // namespace URay
