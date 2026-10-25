#include "EditorWidgetFactory.h"
#include <Core/Type/Types.h>

#include <Editor/Widget/Style/EditorStyles.h>

#include <Engine/UI/Widget/Element/Button.h>
#include <Engine/UI/Widget/Element/Checkbox.h>
#include <Engine/UI/Widget/Element/Image.h>
#include <Engine/UI/Widget/Element/Label.h>
#include <Engine/UI/Widget/Element/TextBox.h>
#include <Engine/UI/Widget/Widget.h>

#include <memory>
#include <string>

namespace URay
{

Label* CreateEditorLabel(Widget& parent, StringView text)
{
    Label* label = parent.AddChild<Label>(text);
    label->SetTextStyle(EditorStyle::SmallText());
    return label;
}

Button* CreateEditorButton(Widget& parent, StringView text)
{
    auto label = std::make_unique<Label>(text);
    label->SetTextStyle(EditorStyle::ButtonText());
    label->SetHorizontalAlignment(HorizontalTextAlignment::Center);
    label->SetVerticalAlignment(VerticalTextAlignment::Center);

    Button* button = parent.AddChild<Button>(std::move(label));
    button->SetNormalColor(EditorStyle::ControlNormal);
    button->SetHoverColor(EditorStyle::ControlHovered);
    button->SetPressedColor(EditorStyle::ControlPressed);
    return button;
}

Button* CreateEditorIconButton(Widget& parent, AssetHandle textureHandle)
{
    Button* button = parent.AddChild<Button>(std::make_unique<Image>(textureHandle));
    button->SetNormalColor(EditorStyle::ControlNormal);
    button->SetHoverColor(EditorStyle::ControlHovered);
    button->SetPressedColor(EditorStyle::ControlPressed);
    return button;
}

Checkbox* CreateEditorCheckbox(Widget& parent)
{
    Checkbox* checkbox = parent.AddChild<Checkbox>();
    ConfigureEditorCheckbox(*checkbox);
    return checkbox;
}

void ConfigureEditorCheckbox(Checkbox& checkbox)
{
    checkbox.SetNormalColor(EditorStyle::ControlNormal);
    checkbox.SetHoverColor(EditorStyle::ControlHovered);
    checkbox.SetPressedColor(EditorStyle::ControlPressed);
    checkbox.SetOutlineColor(EditorStyle::Border);
    checkbox.SetCheckmarkColor(EditorStyle::TextPrimary);
}

TextBox* CreateEditorTextBox(Widget& parent, StringView text)
{
    TextBox* textBox = parent.AddChild<TextBox>(String(text));
    ConfigureEditorTextBox(*textBox);
    return textBox;
}

void ConfigureEditorTextBox(TextBox& textBox)
{
    textBox.SetTextStyle(EditorStyle::SmallText());
    textBox.SetNormalColor(EditorStyle::ControlNormal);
    textBox.SetHoverColor(EditorStyle::ControlHovered);
    textBox.SetFocusedColor(EditorStyle::ControlPressed);
    textBox.SetOutlineColor(EditorStyle::Border);
    textBox.SetOutlineWidth(1.0f);
    textBox.SetSelectionColor(EditorStyle::TextSelection);
    textBox.SetCaretColor(EditorStyle::TextPrimary);
}

} // namespace URay
