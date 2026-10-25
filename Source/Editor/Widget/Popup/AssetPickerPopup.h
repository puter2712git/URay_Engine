#pragma once

#include <Core/Type/Ray/Ray.h>
#include <Editor/Widget/Popup/PopupMenu.h>
#include <Engine/Asset/AssetHandle.h>

namespace URay
{

class Class;

class AssetPickerPopup final : public PopupMenu
{
public:
    explicit AssetPickerPopup(Class* assetClass, AssetHandle currentValue);

public:
    Ray<AssetHandle> onValueSelected;
};

} // namespace URay
