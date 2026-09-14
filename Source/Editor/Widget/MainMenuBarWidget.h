#pragma once

#include "Editor/Widget/Widget.h"

namespace URay
{

class Engine;

class MainMenuBarWidget final : public Widget
{
public:
    MainMenuBarWidget();
    ~MainMenuBarWidget() override;

protected:
    void OnDraw() override;
};

} // namespace URay
