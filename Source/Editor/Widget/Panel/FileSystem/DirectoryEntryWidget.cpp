#include "DirectoryEntryWidget.h"

#include "Editor/Widget/Element/Label.h"

#include "Engine/Asset/EngineAsset.h"

#include <memory>

namespace URay
{

DirectoryEntryWidget::DirectoryEntryWidget(const VirtualPath& path) : path(path)
{
    SetDesiredSize(Vector2(100.0f, 200.0f));

    auto label = std::make_unique<Label>(path.GetStem());
    label->SetTextStyle({
        .fontHandle = EngineAsset::EditorFont,
        .pixelHeight = 12,
        .color = Color::White,
    });
    label->SetDesiredSize(Vector2(100.0f, 200.0f));
    AddChild(std::move(label));
}

void DirectoryEntryWidget::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    children[0]->Arrange(rect);
}

} // namespace URay
