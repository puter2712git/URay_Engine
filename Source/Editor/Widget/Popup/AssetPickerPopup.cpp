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
    AddMenuItem("<Null>", [this]
                { valueSelectedRay.Emit(AssetHandle{}); });

    AssetDatabase& database = gEngine->GetAssetSystem().GetDatabase();
    std::vector<Asset*> assets = database.GetAssets(assetClass);

    std::sort(assets.begin(), assets.end(), [](const Asset* lhs, const Asset* rhs)
              { return lhs->GetName() < rhs->GetName(); });

    for (Asset* asset : assets)
    {
        const AssetHandle handle = asset->GetHandle();

        AddMenuItem(asset->GetName(), [this, handle]
                    { valueSelectedRay.Emit(handle); });
    }
}

} // namespace URay
