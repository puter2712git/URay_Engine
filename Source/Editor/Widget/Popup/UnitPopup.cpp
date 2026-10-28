#include "UnitPopup.h"

namespace URay
{

UnitPopup::UnitPopup()
{
    Register("ItemSelected", this, &UnitPopup::OnMenuItemSelected);
    AddMenuItem("Empty Unit", 0);
}

void UnitPopup::OnMenuItemSelected(int32 id)
{
    if (id == 0)
        onUnitSelected.Emit(UnitPopupSelection::EmptyUnit);
}

} // namespace URay
