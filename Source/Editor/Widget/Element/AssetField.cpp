#include "AssetField.h"

#include <Editor/Widget/Element/TextBox.h>

#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Engine.h>

namespace URay
{

AssetField::AssetField(Class* assetClass) : assetClass(assetClass)
{
    textBox = AddChild<TextBox>("None");
    textBox->SetReadonly(true);
}

void AssetField::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    textBox->Arrange(rect);
}

void AssetField::SetValue(AssetHandle handle)
{
    value = handle;

    AssetDatabase& database = gEngine->GetAssetSystem().GetDatabase();

    Asset* asset = database.Find(value, assetClass);
    textBox->SetText(asset ? asset->GetName() : "None");
}

Vector2 AssetField::OnMeasure()
{
    return textBox->GetDesiredSize();
}

} // namespace URay
