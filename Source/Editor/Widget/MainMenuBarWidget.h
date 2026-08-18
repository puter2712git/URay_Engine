#pragma once

#include "Editor/Widget/Widget.h"

namespace URay
{

class Engine;

class MainMenuBarWidget final : public Widget
{
public:
    MainMenuBarWidget(Engine& engine);
    ~MainMenuBarWidget() override;

protected:
    void OnDraw() override;

private:
    Engine& engine;
};

} // namespace URay
