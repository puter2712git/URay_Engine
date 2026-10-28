#pragma once

#include <Core/Type/Ray/Ray.h>
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

private:
    void OnMenuItemSelected(int32 id);

public:
    Ray<UnitPopupSelection> onUnitSelected;
};

} // namespace URay
