#include "ImportAssetDialog.h"

#include <Core/File/VirtualFileSystem.h>
#include <Core/Math/Math.h>
#include <Editor/Widget/EditorWidgetFactory.h>
#include <Editor/Widget/Panel/FileSystem/BreadcrumbBar.h>
#include <Editor/Widget/Panel/FileSystem/FileGridView.h>
#include <Editor/Widget/Panel/ScrollView.h>
#include <Editor/Widget/Style/EditorStyles.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Importer/ImportRequest.h>
#include <Engine/UI/Widget/Element/Button.h>
#include <Engine/UI/Widget/Element/Label.h>
#include <Engine/UI/Widget/Element/Surface.h>

namespace URay
{

namespace
{

constexpr float DialogWidth = 640.0f;
constexpr float DialogHeight = 480.0f;
constexpr float BreadcrumbHeight = 28.0f;
constexpr float LabelHeight = 20.0f;
constexpr float ActionHeight = 28.0f;
constexpr float Gap = 8.0f;

} // namespace

ImportAssetDialog::ImportAssetDialog(AssetSystem& assetSystem, const VirtualPath& outputDirectory)
    : assetSystem(assetSystem), fileSystem(assetSystem.GetFileSystem()), outputDirectory(outputDirectory)
{
    background = AddChild<Surface>();
    background->SetBackgroundColor(EditorStyle::Surface);
    background->SetOutlineColor(EditorStyle::Border);
    background->SetOutlineWidth(1.0f);
    background->SetPadding(Padding(16.0f));

    breadcrumbBar = AddChild<BreadcrumbBar>();

    contentScroll = AddChild<ScrollView>();
    fileGrid = contentScroll->SetContent<FileGridView>();

    destinationLabel = CreateEditorLabel(*this);
    destinationLabel->SetText("Double-click a source file to import to: " + outputDirectory.ToString());
    destinationLabel->SetTextStyle(EditorStyle::SmallText());

    validationLabel = CreateEditorLabel(*this);
    validationLabel->SetTextStyle(EditorStyle::SmallText());

    cancelButton = CreateEditorButton(*this, "Cancel");
    cancelButton->SetPreferredSize(Vector2(80.0f, ActionHeight));

    breadcrumbBar->onPathActivated.Register(
        this,
        [this](const VirtualPath& path)
        {
            pendingDirectory = path;
        });

    fileGrid->Register("EntryActivated", this, &ImportAssetDialog::OnFileEntryActivated);

    cancelButton->Register("Clicked", this, &ImportAssetDialog::Cancel);

    NavigateTo(currentDirectory);
}

void ImportAssetDialog::OnFileEntryActivated(const VirtualFileEntry* entry)
{
    if (!entry)
        return;

    if (entry->isDirectory)
        pendingDirectory = entry->path;
    else
        Import(entry->path);
}

void ImportAssetDialog::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);
    background->Arrange(rect);

    const Rect contentRect = background->GetContentRect();

    Rect breadcrumbRect;
    breadcrumbRect.position = contentRect.position;
    breadcrumbRect.size = Vector2(contentRect.size.x, BreadcrumbHeight);
    breadcrumbBar->Arrange(breadcrumbRect);

    Rect cancelRect;
    cancelRect.position = Vector2(
        contentRect.position.x + Math::Max(0.0f, contentRect.size.x - 80.0f),
        contentRect.position.y + contentRect.size.y - ActionHeight);
    cancelRect.size = Vector2(80.0f, ActionHeight);
    cancelButton->Arrange(cancelRect);

    Rect validationRect;
    validationRect.position = Vector2(
        contentRect.position.x,
        cancelRect.position.y - Gap - LabelHeight);
    validationRect.size = Vector2(contentRect.size.x, LabelHeight);
    validationLabel->Arrange(validationRect);

    Rect destinationRect;
    destinationRect.position = Vector2(
        contentRect.position.x,
        validationRect.position.y - Gap - LabelHeight);
    destinationRect.size = Vector2(contentRect.size.x, LabelHeight);
    destinationLabel->Arrange(destinationRect);

    Rect gridRect;
    gridRect.position = Vector2(
        contentRect.position.x,
        breadcrumbRect.position.y + breadcrumbRect.size.y + Gap);
    gridRect.size = Vector2(
        contentRect.size.x,
        Math::Max(0.0f, destinationRect.position.y - Gap - gridRect.position.y));
    contentScroll->Arrange(gridRect);
}

Vector2 ImportAssetDialog::OnMeasure()
{
    return Vector2(DialogWidth, DialogHeight);
}

void ImportAssetDialog::OnUpdate(float)
{
    if (!pendingDirectory)
        return;

    const VirtualPath path = *pendingDirectory;
    pendingDirectory.reset();
    NavigateTo(path);
}

void ImportAssetDialog::NavigateTo(const VirtualPath& path)
{
    if (!fileSystem.IsDirectory(path))
        return;

    currentDirectory = path;
    breadcrumbBar->SetPath(currentDirectory);
    fileGrid->SetDirectory(fileSystem, currentDirectory);
    SetValidationMessage("");
}

void ImportAssetDialog::Import(const VirtualPath& sourcePath)
{
    if (!assetSystem.CanImport(sourcePath))
    {
        SetValidationMessage("This file type is not supported by an importer.");
        return;
    }

    const ImportRequest request = {
        .sourcePath = sourcePath,
        .outputDirectory = outputDirectory,
    };

    if (!assetSystem.Import(request))
    {
        SetValidationMessage("Import failed. See the log for details.");
        return;
    }

    Accept();
}

void ImportAssetDialog::SetValidationMessage(const char* message)
{
    validationLabel->SetText(message);
}

} // namespace URay
