#include "UnitPopup.h"

namespace URay
{

UnitPopup::UnitPopup()
{
    AddMenuItem("Empty Unit", [this]
                { unitSelectedRay.Emit(UnitPopupSelection::EmptyUnit); });
}

} // namespace URay
