#include "EditorLayout.h"

#include <Editor/Widget/Docking/DockTabGroup.h>
#include <Editor/Widget/Element/Splitter.h>
#include <Editor/Widget/Panel/PanelFactory.h>
#include <Engine/UI/UISystem.h>

#include <cmath>
#include <unordered_set>

namespace URay
{
namespace
{
constexpr int layoutVersion = 1;
constexpr size_t maxDepth = 32;
const std::unordered_set<String> requiredPanels = {
    "SceneTreePanel", "InspectorPanel", "ConsolePanel", "FileSystemPanel", "StatusPanel", "ViewportPanel"
};

bool ValidateNode(const YAML::Node& node, std::unordered_set<String>& panels, size_t depth)
{
    if (depth > maxDepth || !node.IsMap() || !node["Kind"].IsScalar())
        return false;

    const String kind = node["Kind"].as<String>();
    if (kind == "Splitter")
    {
        if (!node["Axis"].IsScalar() || !node["Ratio"].IsScalar())
            return false;
        const String axis = node["Axis"].as<String>();
        const float ratio = node["Ratio"].as<float>();
        return (axis == "Horizontal" || axis == "Vertical") && std::isfinite(ratio) &&
            ratio >= 0.0f && ratio <= 1.0f &&
            ValidateNode(node["First"], panels, depth + 1) &&
            ValidateNode(node["Second"], panels, depth + 1);
    }
    if (kind != "TabGroup" || !node["Panels"].IsSequence() || node["Panels"].size() == 0 ||
        !node["ActivePanel"].IsScalar())
        return false;

    const String active = node["ActivePanel"].as<String>();
    bool foundActive = false;
    for (const auto& panel : node["Panels"])
    {
        if (!panel.IsScalar())
            return false;
        const String type = panel.as<String>();
        if (!requiredPanels.contains(type) || !PanelFactory::Find(type) || !panels.insert(type).second)
            return false;
        foundActive |= type == active;
    }
    return foundActive;
}

bool ValidateLayout(const YAML::Node& layout)
{
    if (!layout.IsMap() || !layout["Version"].IsScalar() || layout["Version"].as<int>() != layoutVersion)
        return false;
    std::unordered_set<String> panels;
    return ValidateNode(layout["Root"], panels, 0) && panels.size() == requiredPanels.size();
}

YAML::Node SaveNode(const Widget& widget, size_t depth)
{
    if (depth > maxDepth)
        return {};
    YAML::Node node;
    if (const auto* splitter = Cast<const Splitter>(&widget))
    {
        const auto& children = splitter->GetChildren();
        if (children.size() != 2)
            return {};
        node["Kind"] = "Splitter";
        node["Axis"] = splitter->GetAxis() == SplitAxis::Horizontal ? "Horizontal" : "Vertical";
        node["Ratio"] = splitter->GetSplitRatio();
        node["First"] = SaveNode(*children[0], depth + 1);
        node["Second"] = SaveNode(*children[1], depth + 1);
    }
    else if (const auto* group = Cast<const DockTabGroup>(&widget))
    {
        node["Kind"] = "TabGroup";
        node["Panels"] = YAML::Node(YAML::NodeType::Sequence);
        for (const DockedTab& tab : group->GetTabs())
        {
            if (!Cast<const Panel>(tab.panel))
                return {};
            const String& type = tab.panel->GetType().name;
            node["Panels"].push_back(type);
            if (group->GetActiveTabId() == tab.panel->GetId())
                node["ActivePanel"] = type;
        }
    }
    return node;
}

std::unique_ptr<Widget> BuildNode(const YAML::Node& node, UISystem& uiSystem, PanelContext& context)
{
    if (node["Kind"].as<String>() == "Splitter")
    {
        auto first = BuildNode(node["First"], uiSystem, context);
        auto second = BuildNode(node["Second"], uiSystem, context);
        if (!first || !second)
            return nullptr;
        const auto axis = node["Axis"].as<String>() == "Horizontal" ? SplitAxis::Horizontal : SplitAxis::Vertical;
        auto splitter = std::make_unique<Splitter>("", axis, std::move(first), std::move(second));
        splitter->SetSplitRatio(node["Ratio"].as<float>());
        return splitter;
    }

    auto group = std::make_unique<DockTabGroup>(uiSystem);
    const String active = node["ActivePanel"].as<String>();
    ObjectId activeId{};
    for (const auto& panelNode : node["Panels"])
    {
        const String type = panelNode.as<String>();
        const PanelRegistration* registration = PanelFactory::Find(type);
        auto panel = PanelFactory::Create(type, context);
        if (!registration || !panel)
            return nullptr;
        if (type == active)
            activeId = panel->GetId();
        group->AddTab(registration->title, std::move(panel));
    }
    if (!group->SetInitialActiveTab(activeId))
        return nullptr;
    return group;
}
} // namespace

bool EditorLayout::SaveLayout(const Widget& root, YAML::Node& outLayout) const
{
    try
    {
        YAML::Node layout;
        layout["Version"] = layoutVersion;
        layout["Root"] = SaveNode(root, 0);
        if (!ValidateLayout(layout))
            return false;
        outLayout = layout;
        return true;
    }
    catch (const YAML::Exception&)
    {
        return false;
    }
}

std::unique_ptr<Widget> EditorLayout::LoadLayout(const YAML::Node& layout, UISystem& uiSystem, PanelContext& context) const
{
    try
    {
        if (!ValidateLayout(layout))
            return nullptr;
        return BuildNode(layout["Root"], uiSystem, context);
    }
    catch (const YAML::Exception&)
    {
        return nullptr;
    }
}

} // namespace URay
