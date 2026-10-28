#pragma once

#include <Editor/Widget/Popup/PopupMenu.h>
#include <Engine/Asset/AssetHandle.h>

#include <memory>

namespace URay
{

class Class;

class AssetPickerPopup final : public PopupMenu
{
public:
    explicit AssetPickerPopup(Class* assetClass, AssetHandle currentValue);
    std::weak_ptr<void> GetLifetimeToken() const { return lifetimeToken; }

private:
    void OnMenuItemSelected(int32 id);
    std::vector<AssetHandle> handles;
    std::shared_ptr<void> lifetimeToken = std::make_shared<int>(0);
};

} // namespace URay
