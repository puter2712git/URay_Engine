#pragma once

#include <Editor/Widget/Popup/PopupMenu.h>

namespace URay
{

enum class AssetCreateSelection
{
    Script
};
URAY_ENUM_TYPE(AssetCreateSelection)

class AssetCreatePopup : public PopupMenu
{
public:
    AssetCreatePopup();

private:
    void OnMenuItemSelected(int32 id);
};

} // namespace URay
