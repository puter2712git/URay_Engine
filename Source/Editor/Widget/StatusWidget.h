#pragma once

#include "Editor/Widget/Widget.h"

namespace URay
{

class Engine;

class StatusWidget final : public Widget
{
public:
    StatusWidget(Engine& engine);
    ~StatusWidget() override;

protected:
    void OnDraw() override;

private:
    Engine& engine;
};

} // namespace URay
