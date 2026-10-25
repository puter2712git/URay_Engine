#pragma once

#include <Core/Type/Ray.h>
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
    Ray<UnitPopupSelection> onUnitSelected;
};

} // namespace URay
