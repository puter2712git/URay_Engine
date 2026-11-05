#include "CreateScriptDialog.h"

#include <Editor/Widget/EditorWidgetFactory.h>
#include <Editor/Widget/Element/StringField.h>
#include <Editor/Widget/Style/EditorStyles.h>
#include <Engine/UI/Widget/Element/Button.h>
#include <Engine/UI/Widget/Element/Label.h>
#include <Engine/UI/Widget/Element/Surface.h>
#include <Engine/UI/Widget/Layout/VerticalLayout.h>

namespace URay
{

CreateScriptDialog::CreateScriptDialog()
{
    background = AddChild<Surface>();
    background->SetBackgroundColor(EditorStyle::Surface);
    background->SetOutlineColor(EditorStyle::Border);
    background->SetOutlineWidth(1.0f);
    background->SetPadding(Padding(16.0f));

    layout = background->AddChild<VerticalLayout>();
    layout->SetGap(8.0f);
    layout->SetCrossAxisAlignment(CrossAxisAlignment::Stretch);

    CreateEditorLabel(*layout, "Script Name");

    nameField = layout->AddChild<StringField>();
    nameField->SetPreferredHeight(28.0f);
    nameField->SetText("NewScript");

    validationLabel = CreateEditorLabel(*layout);
    validationLabel->SetTextStyle(EditorStyle::SmallText());
    validationLabel->SetPreferredHeight(20.0f);

    Button* createButton = CreateEditorButton(*layout, "Create");
    createButton->SetPreferredHeight(28.0f);
    createButton->Register("Clicked", this, &CreateScriptDialog::Accept);

    Button* cancelButton = CreateEditorButton(*layout, "Cancel");
    cancelButton->SetPreferredHeight(28.0f);
    cancelButton->Register("Clicked", this, &CreateScriptDialog::Cancel);
}

void CreateScriptDialog::SetCreateHandler(std::function<String(const String&)> handler)
{
    createHandler = std::move(handler);
}

void CreateScriptDialog::Accept()
{
    if (GetResult() != DialogResult::None || !createHandler)
        return;
    const String error = createHandler(GetScriptName());
    if (!error.empty())
    {
        validationLabel->SetText(error);
        return;
    }
    Dialog::Accept();
}

void CreateScriptDialog::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);
    background->Arrange(rect);
    layout->Arrange(background->GetContentRect());
}

const String& CreateScriptDialog::GetScriptName() const
{
    return nameField->GetText();
}

Vector2 CreateScriptDialog::OnMeasure()
{
    return Vector2(440.0f, 190.0f);
}

} // namespace URay
