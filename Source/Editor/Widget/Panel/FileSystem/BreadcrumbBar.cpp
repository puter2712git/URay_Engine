#include "BreadcrumbBar.h"

#include <Editor/Widget/EditorWidgetFactory.h>
#include <Editor/Widget/Style/EditorStyles.h>

#include <Engine/UI/Widget/Element/Label.h>
#include <Engine/UI/Widget/Layout/HorizontalLayout.h>
#include <Editor/Widget/Panel/FileSystem/BreadcrumbItem.h>

namespace URay
{

BreadcrumbBar::BreadcrumbBar()
{
    layout = AddChild<HorizontalLayout>();
    layout->SetGap(2.0f);
    layout->SetCrossAxisAlignment(CrossAxisAlignment::Center);
}

void BreadcrumbBar::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    layout->Arrange(rect);
}

void BreadcrumbBar::SetPath(const VirtualPath& path)
{
    if (currentPath.ToString() == path.ToString())
        return;

    currentPath = path;
    RebuildItems();
}

Vector2 BreadcrumbBar::OnMeasure()
{
    return layout->GetDesiredSize();
}

void BreadcrumbBar::RebuildItems()
{
    layout->ClearChildren();

    const std::string mountName = currentPath.GetMountName();

    if (mountName.empty())
        return;

    VirtualPath accumulatedPath(mountName + "://");

    auto addBreadcrumbItem = [this](const VirtualPath& path)
    {
        BreadcrumbItem* item = layout->AddChild<BreadcrumbItem>(path);

        item->GetOnActivatedRay().Register(
            this,
            [this](const VirtualPath& activatedPath)
            {
                onPathActivated.Emit(activatedPath);
            });
    };

    addBreadcrumbItem(accumulatedPath);

    const std::string relativePath = currentPath.GetRelativePath();

    size_t segmentStart = 0;

    while (segmentStart < relativePath.size())
    {
        const size_t separatorIndex = relativePath.find('/', segmentStart);

        const size_t segmentLength =
            separatorIndex == std::string::npos
                ? std::string::npos
                : separatorIndex - segmentStart;

        const std::string segment = relativePath.substr(segmentStart, segmentLength);

        if (!segment.empty())
        {
            Label* separator = CreateEditorLabel(*layout, ">");
            separator->SetTextStyle(EditorStyle::SmallText());

            accumulatedPath = accumulatedPath.Join(segment);
            addBreadcrumbItem(accumulatedPath);
        }

        if (separatorIndex == std::string::npos)
            break;

        segmentStart = separatorIndex + 1;
    }
}

} // namespace URay
