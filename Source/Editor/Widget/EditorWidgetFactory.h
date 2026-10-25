#pragma once
#include <Core/Type/Types.h>

#include <Engine/Asset/AssetHandle.h>


namespace URay
{

class Button;
class Checkbox;
class Label;
class TextBox;
class Widget;

Label* CreateEditorLabel(Widget& parent, StringView text = {});
Button* CreateEditorButton(Widget& parent, StringView text);
Button* CreateEditorIconButton(Widget& parent, AssetHandle textureHandle);
Checkbox* CreateEditorCheckbox(Widget& parent);
void ConfigureEditorCheckbox(Checkbox& checkbox);
TextBox* CreateEditorTextBox(Widget& parent, StringView text = {});
void ConfigureEditorTextBox(TextBox& textBox);

} // namespace URay
