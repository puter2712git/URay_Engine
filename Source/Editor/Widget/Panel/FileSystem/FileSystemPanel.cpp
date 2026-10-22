#include "FileSystemPanel.h"
#include <Core/Math/Math.h>

#include <Core/File/VirtualFileSystem.h>
#include <Editor/Editor.h>
#include <Editor/Selection/SelectionSystem.h>
#include <Editor/Widget/Element/Separator.h>
#include <Editor/Widget/Layout/VerticalLayout.h>
#include <Editor/Widget/Panel/FileSystem/AssetGridView.h>
#include <Editor/Widget/Panel/FileSystem/BreadcrumbBar.h>
#include <Editor/Widget/Panel/FileSystem/DirectoryTreeView.h>
#include <Editor/Widget/Panel/Inspector/InspectSystem.h>
#include <Editor/Widget/Panel/ScrollView.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Engine.h>
#include <Engine/Scene/SceneSystem.h>

namespace URay
{

FileSystemPanel::FileSystemPanel() : Panel("File System")
{
    VirtualFileSystem& fileSystem = gEngine->GetAssetSystem().GetFileSystem();

    directoryScroll = AddChild<ScrollView>();
    directoryTree = directoryScroll->SetContent<DirectoryTreeView>(fileSystem, VirtualPath("Project://Asset/Source"));

    breadcrumbBar = AddChild<BreadcrumbBar>();

    contentScroll = AddChild<ScrollView>();
    assetGrid = contentScroll->SetContent<AssetGridView>();

    verticalSeparator = AddChild<Separator>(SeparatorOrientation::Vertical);
    horizontalSeparator = AddChild<Separator>(SeparatorOrientation::Horizontal);

    NavigateTo(currentPath);

    assetGrid->GetOnEntryActivatedRay().Register(
        this,
        [this](const AssetEntry& entry)
        {
            if (entry.kind == AssetEntryKind::Directory)
            {
                gEditor->GetInspectSystem().ClearTarget();
                pendingPath = entry.path;
            }
            else if (entry.kind == AssetEntryKind::Asset)
            {
                gEditor->GetSelectionSystem().ClearSelection();
                gEditor->GetInspectSystem().SetTarget(InspectTarget::FromAsset(entry.assetHandle));
            }
            else if (entry.kind == AssetEntryKind::Scene)
            {
                gEditor->LoadGameScene(entry.path);
            }
        });

    breadcrumbBar->GetOnPathActivatedRay().Register(
        this,
        [this](const VirtualPath& path)
        {
            pendingPath = path;
        });

    directoryTree->GetOnDirectoryActivatedRay()
        .Register(
            this,
            [this](const VirtualPath& path)
            {
                pendingPath = path;
            });
}

void FileSystemPanel::Arrange(const Rect& rect)
{
    Panel::Arrange(rect);

    const Rect contentRect = GetContentRect();

    constexpr float directoryWidth = 200.0f;
    constexpr float breadcrumbHeight = 28.0f;
    constexpr float separatorExtent = 8.0f;

    const float leftWidth = Math::Min(directoryWidth, contentRect.size.x);
    const float remainingWidth = Math::Max(0.0f, contentRect.size.x - leftWidth);
    const float verticalSeparatorWidth = Math::Min(separatorExtent, remainingWidth);
    const float rightWidth = Math::Max(0.0f, remainingWidth - verticalSeparatorWidth);

    Rect leftRect;
    leftRect.position = contentRect.position;
    leftRect.size = Vector2(leftWidth, contentRect.size.y);

    Rect verticalSeparatorRect;
    verticalSeparatorRect.position = Vector2(contentRect.position.x + leftWidth, contentRect.position.y);
    verticalSeparatorRect.size = Vector2(verticalSeparatorWidth, contentRect.size.y);

    Rect rightRect;
    rightRect.position = Vector2(verticalSeparatorRect.position.x + verticalSeparatorRect.size.x, contentRect.position.y);
    rightRect.size = Vector2(rightWidth, contentRect.size.y);

    const float actualBreadcrumbHeight = Math::Min(breadcrumbHeight, rightRect.size.y);
    const float remainingHeight = Math::Max(0.0f, rightRect.size.y - actualBreadcrumbHeight);
    const float horizontalSeparatorHeight = Math::Min(separatorExtent, remainingHeight);

    Rect breadcrumbRect;
    breadcrumbRect.position = rightRect.position;
    breadcrumbRect.size = Vector2(rightRect.size.x, actualBreadcrumbHeight);

    Rect horizontalSeparatorRect;
    horizontalSeparatorRect.position = Vector2(rightRect.position.x, breadcrumbRect.position.y + breadcrumbRect.size.y);
    horizontalSeparatorRect.size = Vector2(rightRect.size.x, horizontalSeparatorHeight);

    Rect gridRect;
    gridRect.position = Vector2(rightRect.position.x, horizontalSeparatorRect.position.y + horizontalSeparatorRect.size.y);
    gridRect.size = Vector2(rightRect.size.x, Math::Max(0.0f, rightRect.size.y - breadcrumbRect.size.y - horizontalSeparatorRect.size.y));

    directoryScroll->Arrange(leftRect);
    verticalSeparator->Arrange(verticalSeparatorRect);

    breadcrumbBar->Arrange(breadcrumbRect);
    horizontalSeparator->Arrange(horizontalSeparatorRect);

    contentScroll->Arrange(gridRect);
}

void FileSystemPanel::NavigateTo(const VirtualPath& path)
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();

    currentPath = path;
    directoryTree->SetSelectedPath(currentPath);
    breadcrumbBar->SetPath(currentPath);
    assetGrid->SetDirectory(assetSystem, currentPath);
}

void FileSystemPanel::OnUpdate(float deltaTime)
{
    if (!pendingPath.has_value())
        return;

    NavigateTo(*pendingPath);
    pendingPath.reset();
}

} // namespace URay
