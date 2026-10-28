#include "AssetPickerPopup.h"

#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Engine.h>

#include <algorithm>
#include <vector>

namespace URay
{

AssetPickerPopup::AssetPickerPopup(Class* assetClass, AssetHandle currentValue)
{
    URAY_DECLARE_RAY(ValueSelected, AssetHandle);
    Register("ItemSelected", this, &AssetPickerPopup::OnMenuItemSelected);
    handles.push_back(AssetHandle{});
    AddMenuItem("<Null>", 0);

    AssetDatabase& database = gEngine->GetAssetSystem().GetDatabase();
    std::vector<Asset*> assets = database.GetAssets(assetClass);

    std::sort(assets.begin(), assets.end(), [](const Asset* lhs, const Asset* rhs)
              { return lhs->GetName() < rhs->GetName(); });

    for (Asset* asset : assets)
    {
        const AssetHandle handle = asset->GetHandle();

        const int32 id = static_cast<int32>(handles.size());
        handles.push_back(handle);
        AddMenuItem(asset->GetName(), id);
    }
}

void AssetPickerPopup::OnMenuItemSelected(int32 id)
{
    if (id >= 0 && static_cast<size_t>(id) < handles.size())
        Emit("ValueSelected", handles[static_cast<size_t>(id)]);
}

} // namespace URay
