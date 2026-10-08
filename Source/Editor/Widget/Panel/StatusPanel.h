#pragma once

#include <Editor/Widget/Panel/Panel.h>

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
    VerticalLayout* layout = nullptr;

    Label* fpsLabel = nullptr;
};

} // namespace URay
