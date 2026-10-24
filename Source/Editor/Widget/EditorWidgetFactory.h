#pragma once

#include <Engine/Asset/AssetHandle.h>

#include <string_view>

namespace URay
{

class Button;
class Checkbox;
class Label;
class TextBox;
class Widget;

Label* CreateEditorLabel(Widget& parent, std::string_view text = {});
Button* CreateEditorButton(Widget& parent, std::string_view text);
Button* CreateEditorIconButton(Widget& parent, AssetHandle textureHandle);
Checkbox* CreateEditorCheckbox(Widget& parent);
void ConfigureEditorCheckbox(Checkbox& checkbox);
TextBox* CreateEditorTextBox(Widget& parent, std::string_view text = {});
void ConfigureEditorTextBox(TextBox& textBox);

} // namespace URay
