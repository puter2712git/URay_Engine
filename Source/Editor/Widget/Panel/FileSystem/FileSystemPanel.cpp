#include "FileSystemPanel.h"

#include <Core/File/VirtualFileSystem.h>
#include <Core/Log/LogSystem.h>
#include <Core/Math/Math.h>
#include <Editor/Asset/AssetEditorSystem.h>
#include <Editor/Editor.h>
#include <Editor/Selection/SelectionSystem.h>
#include <Editor/Widget/Dialog/CreateScriptDialog.h>
#include <Editor/Widget/Dialog/ImportAssetDialog.h>
#include <Editor/Widget/EditorUI.h>
#include <Editor/Widget/EditorWidgetFactory.h>
#include <Editor/Widget/Element/Separator.h>
#include <Editor/Widget/Layer/DialogLayer.h>
#include <Editor/Widget/Layer/PopupLayer.h>
#include <Editor/Widget/Panel/FileSystem/AssetGridView.h>
#include <Editor/Widget/Panel/FileSystem/BreadcrumbBar.h>
#include <Editor/Widget/Panel/FileSystem/DirectoryTreeView.h>
#include <Editor/Widget/Panel/Inspector/InspectSystem.h>
#include <Editor/Widget/Panel/PanelFactory.h>
#include <Editor/Widget/Panel/ScrollView.h>
#include <Editor/Widget/Popup/AssetCreatePopup.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Metadata/AssetMetadataSerializer.h>
#include <Engine/Asset/Script/Script.h>
#include <Engine/Engine.h>
#include <Engine/Scene/SceneSystem.h>
#include <Engine/UI/Widget/Element/Button.h>
#include <Engine/UI/Widget/Layout/VerticalLayout.h>

namespace URay
{

URAY_REGISTER_PANEL(FileSystemPanel, "File System")

FileSystemPanel::FileSystemPanel()
{
    VirtualFileSystem& fileSystem = gEngine->GetAssetSystem().GetFileSystem();

    directoryScroll = AddChild<ScrollView>();
    directoryTree = directoryScroll->SetContent<DirectoryTreeView>(fileSystem, VirtualPath("Project://Asset"));

    breadcrumbBar = AddChild<BreadcrumbBar>();
    importButton = CreateEditorButton(*this, "Import");
    importButton->SetPreferredSize(Vector2(80.0f, 28.0f));

    contentScroll = AddChild<ScrollView>();
    assetGrid = contentScroll->SetContent<AssetGridView>();

    verticalSeparator = AddChild<Separator>(SeparatorOrientation::Vertical);
    horizontalSeparator = AddChild<Separator>(SeparatorOrientation::Horizontal);

    NavigateTo(currentPath);

    assetGrid->Register("EntryClicked", this, &FileSystemPanel::OnEntryClicked);
    assetGrid->Register("EntryDoubleClicked", this, &FileSystemPanel::OnEntryDoubleClicked);

    breadcrumbBar->Register("PathClicked", this, &FileSystemPanel::OnPathClicked);
    directoryTree->Register("DirectoryClicked", this, &FileSystemPanel::OnPathClicked);

    importButton->Register("Clicked", this, &FileSystemPanel::OpenImportDialog);
}

void FileSystemPanel::OnEntryClicked(AssetEntry entry)
{
    if (entry.kind == AssetEntryKind::Asset)
    {
        gEditor->GetSelectionSystem().ClearSelection();
        gEditor->GetInspectSystem().SetTarget(InspectTarget::FromAsset(entry.assetHandle));
    }
}

void FileSystemPanel::OnEntryDoubleClicked(AssetEntry entry)
{
    if (entry.kind == AssetEntryKind::Directory)
    {
        pendingPath = entry.path;
    }
    else if (entry.kind == AssetEntryKind::Asset)
    {
        gEditor->GetAssetEditorSystem().RequestOpen(entry.assetHandle);
    }
    else if (entry.kind == AssetEntryKind::Scene)
    {
        gEditor->LoadGameScene(entry.path);
    }
}

void FileSystemPanel::OnPathClicked(VirtualPath path)
{
    pendingPath = path;
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

    constexpr float importButtonWidth = 80.0f;
    constexpr float importButtonGap = 8.0f;

    Rect breadcrumbRect;
    breadcrumbRect.position = rightRect.position;
    breadcrumbRect.size = Vector2(
        Math::Max(0.0f, rightRect.size.x - importButtonWidth - importButtonGap),
        actualBreadcrumbHeight);

    Rect importButtonRect;
    importButtonRect.position = Vector2(
        breadcrumbRect.position.x + breadcrumbRect.size.x + importButtonGap,
        rightRect.position.y);
    importButtonRect.size = Vector2(importButtonWidth, actualBreadcrumbHeight);

    Rect horizontalSeparatorRect;
    horizontalSeparatorRect.position = Vector2(rightRect.position.x, breadcrumbRect.position.y + breadcrumbRect.size.y);
    horizontalSeparatorRect.size = Vector2(rightRect.size.x, horizontalSeparatorHeight);

    Rect gridRect;
    gridRect.position = Vector2(rightRect.position.x, horizontalSeparatorRect.position.y + horizontalSeparatorRect.size.y);
    gridRect.size = Vector2(rightRect.size.x, Math::Max(0.0f, rightRect.size.y - breadcrumbRect.size.y - horizontalSeparatorRect.size.y));

    directoryScroll->Arrange(leftRect);
    verticalSeparator->Arrange(verticalSeparatorRect);

    breadcrumbBar->Arrange(breadcrumbRect);
    importButton->Arrange(importButtonRect);
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

EventReply FileSystemPanel::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Right)
        return {};

    if (!contentScroll->GetRect().Contains(event.position))
        return {};

    PopupLayer& popupLayer = gEditor->GetEditorUI().GetPopupLayer();
    AssetCreatePopup* popup = popupLayer.OpenPopup<AssetCreatePopup>(event.position);

    popup->Register("MenuSelected", this, &FileSystemPanel::OnAssetCreateSelected);

    return EventReply{
        .handled = true,
        .requestFocus = true
    };
}

void FileSystemPanel::OnUpdate(float deltaTime)
{
    if (!pendingPath.has_value())
        return;

    NavigateTo(*pendingPath);
    pendingPath.reset();
}

void FileSystemPanel::OpenImportDialog()
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();

    ImportAssetDialog* dialog = gEditor->GetEditorUI().GetDialogLayer().OpenDialog<ImportAssetDialog>(
        assetSystem,
        currentPath);

    dialog->Register("Closed", this, &FileSystemPanel::OnImportDialogClosed);
}

void FileSystemPanel::OnImportDialogClosed(DialogResult result)
{
    if (result == DialogResult::Accepted)
        NavigateTo(currentPath);

    gEditor->GetEditorUI().GetDialogLayer().CloseDialog();
}

void FileSystemPanel::OnAssetCreateSelected(AssetCreateSelection selection)
{
    gEditor->GetEditorUI().GetPopupLayer().ClosePopup();

    if (selection == AssetCreateSelection::Script)
    {
        CreateScriptDialog* dialog = gEditor->GetEditorUI().GetDialogLayer().OpenDialog<CreateScriptDialog>();
        dialog->Register("Closed", this, &FileSystemPanel::OnCreateScriptDialogClosed, dialog);
    }
}

void FileSystemPanel::OnCreateScriptDialogClosed(DialogResult result, CreateScriptDialog* dialog)
{
    if (result == DialogResult::Accepted && dialog)
    {
        CreateScriptAsset(dialog->GetScriptName());
    }

    gEditor->GetEditorUI().GetDialogLayer().CloseDialog();
}

bool FileSystemPanel::CreateScriptAsset(const String& name)
{
    if (name.empty() ||
        name == "." ||
        name == ".." ||
        name.find_first_of("/\\:") != String::npos)
    {
        URAY_LOG("[FileSystemPanel] Invalid script path.");
        return false;
    }

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    VirtualFileSystem& fileSystem = assetSystem.GetFileSystem();

    const VirtualPath scriptPath = currentPath.Join(name + ".cs");
    const VirtualPath metadataPath = assetSystem.GetMetadataPath(scriptPath);

    if (fileSystem.Exists(scriptPath) || fileSystem.Exists(metadataPath))
    {
        URAY_LOG("[FileSystemPanel] Script or metadata already exists: %s", scriptPath.ToString().c_str());
        return false;
    }

    // clang-format off
    const String source =
        "using URay;\n"
        "\n"
        "public class " + name + " : ScriptBehaviour\n"
        "{\n"
        "}\n";
    // clang-format on

    AssetMetadata metadata;
    metadata.handle = AssetHandle::Generate();
    metadata.className = Script::StaticType().name;
    metadata.assetPath = scriptPath;
    metadata.sourcePath = scriptPath;

    const auto removeCreatedFile = [&fileSystem](const VirtualPath& path) {
        std::error_code error;
        std::filesystem::remove(fileSystem.ResolveToPhysicalPath(path), error);

        if (error)
        {
            URAY_LOG(
                "[FileSystemPanel] Failed to remove file: %s",
                path.ToString().c_str());
        }
    };

    if (!fileSystem.WriteText(scriptPath, source))
    {
        removeCreatedFile(scriptPath);
        URAY_LOG("[FileSystemPanel] Failed to write script.");
        return false;
    }

    if (!assetSystem.GetMetadataSerializer().Save(metadata, metadataPath))
    {
        removeCreatedFile(metadataPath);
        removeCreatedFile(scriptPath);
        URAY_LOG("[FileSystemPanel] Failed to write script metadata.");
        return false;
    }

    if (!assetSystem.RegisterAsset(metadata))
    {
        removeCreatedFile(metadataPath);
        removeCreatedFile(scriptPath);
        URAY_LOG("[FileSystemPanel] Failed to register script asset.");
        return false;
    }

    NavigateTo(currentPath);
    return true;
}

} // namespace URay
