#pragma once

#include <Editor/Widget/Popup/PopupMenu.h>

namespace URay
{

enum class UnitPopupSelection
{
    EmptyUnit
};

URAY_ENUM_TYPE(UnitPopupSelection)

class UnitPopup final : public PopupMenu
{
public:
    UnitPopup();

private:
    void OnMenuItemSelected(int32 id);

};

} // namespace URay
