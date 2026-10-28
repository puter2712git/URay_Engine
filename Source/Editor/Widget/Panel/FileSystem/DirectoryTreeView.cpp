#include "DirectoryTreeView.h"
#include <Core/Type/TypeAlias.h>

#include <Core/File/VirtualFileSystem.h>
#include <Editor/Widget/Panel/FileSystem/DirectoryTreeItem.h>

#include <algorithm>

namespace URay
{

DirectoryTreeView::DirectoryTreeView(
    VirtualFileSystem& fileSystem, VirtualPath rootPath)
    : fileSystem(fileSystem), rootPath(rootPath), selectedPath(rootPath)
{
    URAY_DECLARE_RAY(DirectoryActivated, VirtualPath);
    expandedPaths.insert(this->rootPath.ToString());
    RebuildItems();
}

void DirectoryTreeView::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    float offsetY = rect.position.y;

    for (const auto& child : children)
    {
        const float height =
            child->GetPreferredHeight().value_or(child->GetDesiredSize().y);

        Rect childRect;
        childRect.position = Vector2(rect.position.x, offsetY);
        childRect.size = Vector2(rect.size.x, height);

        child->Arrange(childRect);

        offsetY += height;
    }
}

void DirectoryTreeView::SetSelectedPath(const VirtualPath& path)
{
    if (selectedPath.ToString() == path.ToString())
        return;

    pendingSelectedPath = path;
}

Vector2 DirectoryTreeView::OnMeasure()
{
    float height = 0.0f;

    for (const auto& child : children)
    {
        height +=
            child->GetPreferredHeight().value_or(child->GetDesiredSize().y);
    }

    return Vector2(0.0f, height);
}

void DirectoryTreeView::OnUpdate(float deltaTime)
{
    bool needsRebuild = false;

    if (pendingExpandPath.has_value())
    {
        const String key = pendingExpandPath->ToString();

        if (expandedPaths.contains(key))
        {
            expandedPaths.erase(key);
        }
        else
        {
            expandedPaths.insert(key);
        }

        pendingExpandPath.reset();
        needsRebuild = true;
    }

    if (pendingSelectedPath.has_value())
    {
        selectedPath = *pendingSelectedPath;
        ExpandAncestors(selectedPath);

        pendingSelectedPath.reset();
        needsRebuild = true;
    }

    if (needsRebuild)
        RebuildItems();
}

void DirectoryTreeView::RebuildItems()
{
    ClearChildren();
    AppendDirectory(rootPath, 0);
}

void DirectoryTreeView::AppendDirectory(const VirtualPath& path, uint32 depth)
{
    const bool expanded = expandedPaths.contains(path.ToString());
    const bool selected = selectedPath.ToString() == path.ToString();

    DirectoryTreeItem* item = AddChild<DirectoryTreeItem>(
        path, depth, expanded, selected);

    item->Register("ExpandRequested", this, &DirectoryTreeView::OnExpandRequested);
    item->Register("Activated", this, &DirectoryTreeView::OnItemActivated);

    if (!expanded)
        return;

    std::vector<VirtualFileEntry> entries = fileSystem.ListDirectory(path);

    entries.erase(
        std::remove_if(
            entries.begin(),
            entries.end(),
            [](const VirtualFileEntry& entry)
            {
                return !entry.isDirectory;
            }),
        entries.end());

    std::sort(
        entries.begin(),
        entries.end(),
        [](const VirtualFileEntry& lhs,
           const VirtualFileEntry& rhs)
        {
            return lhs.path.GetFilename() < rhs.path.GetFilename();
        });

    for (const VirtualFileEntry& entry : entries)
    {
        AppendDirectory(entry.path, depth + 1);
    }
}

void DirectoryTreeView::OnExpandRequested(VirtualPath path)
{
    pendingExpandPath = path;
}

void DirectoryTreeView::OnItemActivated(VirtualPath path)
{
    pendingSelectedPath = path;
    Emit("DirectoryActivated", path);
}

void DirectoryTreeView::ExpandAncestors(const VirtualPath& path)
{
    VirtualPath ancestor = path.GetDirectory();

    while (!ancestor.ToString().empty())
    {
        expandedPaths.insert(ancestor.ToString());

        if (ancestor.ToString() == rootPath.ToString())
            break;

        ancestor = ancestor.GetDirectory();
    }
}

} // namespace URay
