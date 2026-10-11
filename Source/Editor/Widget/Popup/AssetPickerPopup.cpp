#include "AssetPickerPopup.h"

#include <Editor/Widget/Element/Button.h>
#include <Editor/Widget/Layout/VerticalLayout.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Engine.h>

#include <algorithm>
#include <vector>

namespace URay
{

AssetPickerPopup::AssetPickerPopup(Class* assetClass, AssetHandle currentValue)
{
    layout = AddChild<VerticalLayout>();
    layout->SetGap(2.0f);
    layout->SetCrossAxisAlignment(CrossAxisAlignment::Stretch);

    Button* noneButton = layout->AddChild<Button>("None");
    noneButton->SetPreferredHeight(24.0f);
    noneButton->GetOnClickRay().Register(
        this,
        [this]
        {
            valueSelectedRay.Emit(AssetHandle{});
        });

    AssetDatabase& database = gEngine->GetAssetSystem().GetDatabase();
    std::vector<Asset*> assets = database.GetAssets(assetClass);

    std::sort(
        assets.begin(),
        assets.end(),
        [](const Asset* lhs, const Asset* rhs)
        {
            return lhs->GetName() < rhs->GetName();
        });

    for (Asset* asset : assets)
    {
        Button* button = layout->AddChild<Button>(asset->GetName());
        button->SetPreferredHeight(24.0f);

        const AssetHandle handle = asset->GetHandle();

        button->GetOnClickRay().Register(
            this,
            [this, handle]
            {
                valueSelectedRay.Emit(handle);
            });
    }
}

void AssetPickerPopup::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    layout->Arrange(rect);
}

Vector2 AssetPickerPopup::OnMeasure()
{
    return layout->GetDesiredSize();
}

} // namespace URay
