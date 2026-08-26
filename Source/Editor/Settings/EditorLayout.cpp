#include "EditorLayout.h"

#include "Editor/Widget/Splitter.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/File/VirtualPath.h"

namespace URay
{

EditorLayout::EditorLayout()
{
}

YAML::Node EditorLayout::SaveLayout(const Widget& root)
{
    YAML::Node splitters;

    CollectSplitters(root, splitters);

    return splitters;
}

void EditorLayout::LoadLayout(Widget& root, const YAML::Node& layout)
{
    if (layout)
    {
        ApplySplitters(root, layout);
    }
}

void EditorLayout::CollectSplitters(const Widget& widget, YAML::Node& splitters)
{
    if (const auto* splitter = dynamic_cast<const Splitter*>(&widget))
    {
        if (!splitter->GetLayoutId().empty())
        {
            splitters[splitter->GetLayoutId()] = splitter->GetSplitRatio();
        }
    }

    for (const auto& child : widget.GetChildren())
    {
        CollectSplitters(*child, splitters);
    }
}

void EditorLayout::ApplySplitters(Widget& widget, const YAML::Node& layout)
{
    if (auto* splitter = dynamic_cast<Splitter*>(&widget))
    {
        const YAML::Node ratioNode = layout[splitter->GetLayoutId()];
        if (ratioNode)
        {
            splitter->SetSplitRatio(ratioNode.as<float>());
        }
    }

    for (const auto& child : widget.GetChildren())
    {
        ApplySplitters(*child, layout);
    }
}

} // namespace URay
