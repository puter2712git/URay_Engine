#include "FileSystemPanel.h"

#include <Core/File/VirtualFileSystem.h>
#include <Editor/Widget/Layout/VerticalLayout.h>
#include <Editor/Widget/Panel/FileSystem/DirectoryTreeView.h>
#include <Editor/Widget/Panel/FileSystem/FileGridView.h>
#include <Editor/Widget/Panel/ScrollView.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Engine.h>

namespace URay
{

FileSystemPanel::FileSystemPanel() : Panel("File System")
{
    VirtualFileSystem& fileSystem = gEngine->GetAssetSystem().GetFileSystem();

    directoryScroll = AddChild<ScrollView>();
    directoryTree = directoryScroll->SetContent<DirectoryTreeView>(fileSystem, VirtualPath("Project://"));

    // breadcrumbBar = AddChild<BreadcrumbBar>();

    contentScroll = AddChild<ScrollView>();
    fileGrid = contentScroll->SetContent<FileGridView>();

    NavigateTo(currentPath);

    fileGrid->GetOnEntryActivatedRay().Register(
        this,
        [this](const VirtualFileEntry& entry)
        {
            if (entry.isDirectory)
                pendingPath = entry.path;
        });

    directoryTree->GetOnDirectoryActivatedRay().Register(
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

    const float leftWidth = std::min(directoryWidth, contentRect.size.x);

    Rect leftRect;
    leftRect.position = Vector2(contentRect.position.x, contentRect.position.y);
    leftRect.size = Vector2(leftWidth, contentRect.size.y);

    Rect rightRect;
    rightRect.position = Vector2(contentRect.position.x + leftWidth, contentRect.position.y);
    rightRect.size = Vector2(std::max(0.0f, contentRect.size.x - leftWidth), contentRect.size.y);

    Rect breadcrumbRect;
    breadcrumbRect.position = rightRect.position;
    breadcrumbRect.size = Vector2(rightRect.size.x, std::min(breadcrumbHeight, rightRect.size.y));

    Rect gridRect;
    gridRect.position = Vector2(rightRect.position.x, rightRect.position.y + breadcrumbRect.size.y);
    gridRect.size = Vector2(rightRect.size.x, std::max(0.0f, rightRect.size.y - breadcrumbRect.size.y));

    directoryScroll->Arrange(leftRect);
    // breadcrumbBar->Arrange(breadcrumbRect);
    contentScroll->Arrange(gridRect);
}

void FileSystemPanel::NavigateTo(const VirtualPath& path)
{
    VirtualFileSystem& fileSystem = gEngine->GetAssetSystem().GetFileSystem();

    currentPath = path;
    directoryTree->SetSelectedPath(currentPath);
    fileGrid->SetDirectory(fileSystem, currentPath);
}

void FileSystemPanel::OnUpdate(float deltaTime)
{
    if (!pendingPath.has_value())
        return;

    NavigateTo(*pendingPath);
    pendingPath.reset();
}

} // namespace URay
