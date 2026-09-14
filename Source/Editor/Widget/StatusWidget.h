#pragma once

#include "Editor/Widget/Widget.h"

namespace URay
{

class Engine;

class StatusWidget final : public Widget
{
public:
    StatusWidget();
    ~StatusWidget() override;

protected:
    void OnDraw() override;
};

} // namespace URay
