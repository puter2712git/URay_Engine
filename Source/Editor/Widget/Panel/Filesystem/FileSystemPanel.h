#pragma once

#include <Core/File/VirtualPath.h>
#include <Editor/Widget/Panel/Panel.h>

#include <optional>
#include <vector>

namespace URay
{

class ScrollView;
class DirectoryTreeView;
class BreadcrumbBar;
class FileGridView;

class FileSystemPanel final : public Panel
{
    URAY_TYPE(FileSystemPanel, Panel)

public:
    FileSystemPanel();

public:
    void Arrange(const Rect& rect) override;

    void NavigateTo(const VirtualPath& path);

protected:
    void OnUpdate(float deltaTime) override;

private:
    VirtualPath currentPath = "Project://Asset/Source";
    std::optional<VirtualPath> pendingPath = std::nullopt;

    ScrollView* directoryScroll = nullptr;
    DirectoryTreeView* directoryTree = nullptr;

    BreadcrumbBar* breadcrumbBar = nullptr;

    ScrollView* contentScroll = nullptr;
    FileGridView* fileGrid = nullptr;
};

} // namespace URay
