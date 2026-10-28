#include "BreadcrumbBar.h"
#include <Core/Type/TypeAlias.h>

#include <Editor/Widget/EditorWidgetFactory.h>
#include <Editor/Widget/Style/EditorStyles.h>

#include <Engine/UI/Widget/Element/Label.h>
#include <Engine/UI/Widget/Layout/HorizontalLayout.h>
#include <Editor/Widget/Panel/FileSystem/BreadcrumbItem.h>

namespace URay
{

BreadcrumbBar::BreadcrumbBar()
{
    URAY_DECLARE_RAY(PathActivated, VirtualPath);
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

    const String mountName = currentPath.GetMountName();

    if (mountName.empty())
        return;

    VirtualPath accumulatedPath(mountName + "://");

    auto addBreadcrumbItem = [this](const VirtualPath& path)
    {
        BreadcrumbItem* item = layout->AddChild<BreadcrumbItem>(path);

        item->Register("Activated", this, &BreadcrumbBar::OnItemActivated);
    };

    addBreadcrumbItem(accumulatedPath);

    const String relativePath = currentPath.GetRelativePath();

    size_t segmentStart = 0;

    while (segmentStart < relativePath.size())
    {
        const size_t separatorIndex = relativePath.find('/', segmentStart);

        const size_t segmentLength =
            separatorIndex == String::npos
                ? String::npos
                : separatorIndex - segmentStart;

        const String segment = relativePath.substr(segmentStart, segmentLength);

        if (!segment.empty())
        {
            Label* separator = CreateEditorLabel(*layout, ">");
            separator->SetTextStyle(EditorStyle::SmallText());

            accumulatedPath = accumulatedPath.Join(segment);
            addBreadcrumbItem(accumulatedPath);
        }

        if (separatorIndex == String::npos)
            break;

        segmentStart = separatorIndex + 1;
    }
}

void BreadcrumbBar::OnItemActivated(VirtualPath path)
{
    Emit("PathActivated", path);
}

} // namespace URay
