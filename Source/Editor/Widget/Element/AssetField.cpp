#include "AssetField.h"

#include <Editor/Editor.h>
#include <Editor/Widget/Element/TextBox.h>
#include <Editor/Widget/Layer/PopupLayer.h>
#include <Editor/Widget/Popup/AssetPickerPopup.h>
#include <Editor/Widget/WidgetSystem.h>
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

EventReply AssetField::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left)
        return {};

    PopupLayer& popupLayer = gEditor->GetWidgetSystem().GetPopupLayer();

    AssetPickerPopup* popup =
        popupLayer.OpenPopup<AssetPickerPopup>(
            GetRect(),
            assetClass,
            value);

    popup->GetValueSelectedRay().Register(
        this,
        [this, &popupLayer](AssetHandle selectedHandle)
        {
            if (value != selectedHandle)
            {
                SetValue(selectedHandle);
                valueChangedRay.Emit(selectedHandle);
            }

            popupLayer.ClosePopup();
        });

    return EventReply{
        .handled = true,
        .requestFocus = true
    };
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
