#include "AssetCreatePopup.h"

namespace URay
{

AssetCreatePopup::AssetCreatePopup()
{
    URAY_DECLARE_RAY(MenuSelected, AssetCreateSelection);

    Register("ItemSelected", this, &AssetCreatePopup::OnMenuItemSelected);

    AddMenuItem("Script", 0);
}

void AssetCreatePopup::OnMenuItemSelected(int32 id)
{
    if (id == 0)
    {
        Emit("MenuSelected", AssetCreateSelection::Script);
    }
}

} // namespace URay
