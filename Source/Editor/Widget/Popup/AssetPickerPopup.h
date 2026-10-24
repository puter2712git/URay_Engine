#pragma once

#include <Core/Ray/EventRay.h>
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
    EventRay<AssetHandle> onValueSelected;
};

} // namespace URay
