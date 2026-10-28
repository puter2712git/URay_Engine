#include "UnitPopup.h"

namespace URay
{

UnitPopup::UnitPopup()
{
    URAY_DECLARE_RAY(UnitSelected, UnitPopupSelection);
    Register("ItemSelected", this, &UnitPopup::OnMenuItemSelected);
    AddMenuItem("Empty Unit", 0);
}

void UnitPopup::OnMenuItemSelected(int32 id)
{
    if (id == 0)
        Emit("UnitSelected", UnitPopupSelection::EmptyUnit);
}

} // namespace URay
