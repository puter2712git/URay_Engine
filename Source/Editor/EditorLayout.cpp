#include "EditorLayout.h"

#include "Editor/Widget/Splitter.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/File/VirtualPath.h"

namespace URay
{

EditorLayout::EditorLayout(VirtualFilesystem& filesystem)
    : filesystem(filesystem)
{
}

void EditorLayout::SaveLayout(const Widget& root)
{
    YAML::Node splitters;

    CollectSplitters(root, splitters);

    YAML::Node document;
    document["version"] = 1;
    document["splitters"] = splitters;

    filesystem.WriteText("Project://Config/EditorLayout.ini", YAML::Dump(document));
}

void EditorLayout::LoadLayout(Widget& root)
{
    const std::string layoutText = filesystem.ReadText("Project://Config/EditorLayout.ini");
    if (layoutText.empty())
        return;

    YAML::Node layout = YAML::Load(layoutText);

    ApplySplitters(root, layout["splitters"]);
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
