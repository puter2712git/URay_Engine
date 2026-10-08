#pragma once

#include "Editor/Widget/Panel/Panel.h"

namespace URay
{

class Label;
class Button;
class Image;

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
    Label* fpsLabel = nullptr;
    Button* button = nullptr;
    Image* testImage = nullptr;
};

} // namespace URay
