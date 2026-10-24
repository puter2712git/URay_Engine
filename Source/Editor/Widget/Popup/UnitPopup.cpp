#include "UnitPopup.h"

namespace URay
{

UnitPopup::UnitPopup()
{
    AddMenuItem("Empty Unit", [this]
                { onUnitSelected.Emit(UnitPopupSelection::EmptyUnit); });
}

} // namespace URay
