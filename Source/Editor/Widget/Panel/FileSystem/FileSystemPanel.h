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
class AssetGridView;
class Button;
class Separator;
class CreateScriptDialog;
struct AssetEntry;
enum class DialogResult;
enum class AssetCreateSelection;

class FileSystemPanel final : public Panel
{
    URAY_CLASS(FileSystemPanel, Panel)

public:
    FileSystemPanel();

public:
    void Arrange(const Rect& rect) override;

    void NavigateTo(const VirtualPath& path);

    EventReply OnPointerDown(const PointerEvent& event) override;

protected:
    void OnUpdate(float deltaTime) override;

private:
    void OpenImportDialog();

    void OnImportDialogClosed(DialogResult result);
    void OnPathClicked(VirtualPath path);
    void OnEntryClicked(AssetEntry entry);
    void OnEntryDoubleClicked(AssetEntry entry);
    void OnAssetCreateSelected(AssetCreateSelection selection);
    void OnCreateScriptDialogClosed(DialogResult result, CreateScriptDialog* dialog);

    bool CreateScriptAsset(const String& name);

private:
    VirtualPath currentPath = "Project://Asset";
    std::optional<VirtualPath> pendingPath = std::nullopt;

    ScrollView* directoryScroll = nullptr;
    DirectoryTreeView* directoryTree = nullptr;

    BreadcrumbBar* breadcrumbBar = nullptr;
    Button* importButton = nullptr;

    ScrollView* contentScroll = nullptr;
    AssetGridView* assetGrid = nullptr;

    Separator* verticalSeparator = nullptr;
    Separator* horizontalSeparator = nullptr;
};

} // namespace URay
