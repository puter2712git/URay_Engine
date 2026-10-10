#include "DirectoryEntryWidget.h"

#include <Editor/Widget/Style/EditorStyles.h>

#include <Editor/Widget/Element/Label.h>

#include <Engine/Asset/EngineAsset.h>

#include <memory>

namespace URay
{

DirectoryEntryWidget::DirectoryEntryWidget(const VirtualPath& path) : path(path)
{
    SetPreferredSize(Vector2(100.0f, 200.0f));

    Label* label = AddChild<Label>(path.GetStem());
    label->SetPreferredSize(Vector2(100.0f, 200.0f));
    label->SetTextStyle({
        .fontHandle = EngineAsset::EditorFont,
        .pixelHeight = 12,
        .color = EditorStyle::TextPrimary,
    });
}

void DirectoryEntryWidget::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    children[0]->Arrange(rect);
}

} // namespace URay
