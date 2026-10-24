#pragma once

#include <Core/Ray/EventRay.h>
#include <Editor/Widget/Popup/PopupMenu.h>

namespace URay
{

enum class UnitPopupSelection
{
    EmptyUnit
};

class UnitPopup final : public PopupMenu
{
public:
    UnitPopup();

public:
    EventRay<UnitPopupSelection>& GetUnitSelectedRay() { return onUnitSelected; }

private:
    EventRay<UnitPopupSelection> onUnitSelected;
};

} // namespace URay
