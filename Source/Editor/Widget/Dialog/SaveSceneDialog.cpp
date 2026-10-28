#include "SaveSceneDialog.h"
#include <Core/Math/Math.h>
#include <Core/Type/TypeAlias.h>

#include <Core/File/VirtualFileSystem.h>
#include <Editor/Widget/EditorWidgetFactory.h>
#include <Editor/Widget/Element/StringField.h>
#include <Editor/Widget/Panel/FileSystem/BreadcrumbBar.h>
#include <Editor/Widget/Panel/FileSystem/FileGridView.h>
#include <Editor/Widget/Panel/ScrollView.h>
#include <Editor/Widget/Style/EditorStyles.h>
#include <Engine/UI/Widget/Element/Button.h>
#include <Engine/UI/Widget/Element/Label.h>
#include <Engine/UI/Widget/Element/Surface.h>
#include <Engine/UI/Widget/Layout/HorizontalLayout.h>

#include <algorithm>

namespace URay
{

namespace
{

constexpr float DialogWidth = 640.0f;
constexpr float DialogHeight = 480.0f;
constexpr float BreadcrumbHeight = 28.0f;
constexpr float FieldHeight = 28.0f;
constexpr float ValidationHeight = 18.0f;
constexpr float ActionHeight = 28.0f;
constexpr float Gap = 8.0f;
constexpr float FilenameLabelWidth = 72.0f;

} // namespace

SaveSceneDialog::SaveSceneDialog(VirtualFileSystem& fileSystem)
    : fileSystem(fileSystem)
{
    background = AddChild<Surface>();
    background->SetBackgroundColor(EditorStyle::Surface);
    background->SetOutlineColor(EditorStyle::Border);
    background->SetOutlineWidth(1.0f);
    background->SetPadding(Padding(16.0f));

    breadcrumbBar = AddChild<BreadcrumbBar>();

    contentScroll = AddChild<ScrollView>();
    fileGrid = contentScroll->SetContent<FileGridView>();

    filenameLabel = CreateEditorLabel(*this, "File name");
    filenameField = AddChild<StringField>();
    filenameField->SetText("Untitled.scene");

    validationLabel = CreateEditorLabel(*this);
    validationLabel->SetTextStyle(EditorStyle::SmallText());

    actionLayout = AddChild<HorizontalLayout>();
    actionLayout->SetGap(Gap);
    actionLayout->SetMainAxisAlignment(MainAxisAlignment::End);
    actionLayout->SetCrossAxisAlignment(CrossAxisAlignment::Center);

    saveButton = CreateEditorButton(*actionLayout, "Save");
    saveButton->SetPreferredSize(Vector2(80.0f, ActionHeight));

    cancelButton = CreateEditorButton(*actionLayout, "Cancel");
    cancelButton->SetPreferredSize(Vector2(80.0f, ActionHeight));

    breadcrumbBar->onPathActivated.Register(
        this,
        [this](const VirtualPath& path)
        {
            pendingDirectory = path;
        });

    fileGrid->Register("EntryActivated", this, &SaveSceneDialog::OnFileEntryActivated);

    filenameField->Register("ValueChanged", this, &SaveSceneDialog::OnFilenameChanged);

    saveButton->Register("Clicked", this, &SaveSceneDialog::Accept);
    cancelButton->Register("Clicked", this, &SaveSceneDialog::Cancel);

    NavigateTo(currentDirectory);
}

void SaveSceneDialog::OnFileEntryActivated(const VirtualFileEntry* entry)
{
    if (entry && entry->isDirectory)
        pendingDirectory = entry->path;
}

void SaveSceneDialog::Accept()
{
    if (!IsValidFilename())
    {
        SetValidationMessage("Enter a filename using the .scene extension.");
        return;
    }

    Dialog::Accept();
}

void SaveSceneDialog::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);
    background->Arrange(rect);

    const Rect contentRect = background->GetContentRect();

    Rect breadcrumbRect;
    breadcrumbRect.position = contentRect.position;
    breadcrumbRect.size = Vector2(contentRect.size.x, BreadcrumbHeight);
    breadcrumbBar->Arrange(breadcrumbRect);

    Rect actionRect;
    actionRect.position = Vector2(
        contentRect.position.x,
        contentRect.position.y + contentRect.size.y - ActionHeight);
    actionRect.size = Vector2(contentRect.size.x, ActionHeight);
    actionLayout->Arrange(actionRect);

    Rect validationRect;
    validationRect.position = Vector2(
        contentRect.position.x,
        actionRect.position.y - Gap - ValidationHeight);
    validationRect.size = Vector2(contentRect.size.x, ValidationHeight);
    validationLabel->Arrange(validationRect);

    Rect filenameLabelRect;
    filenameLabelRect.position = Vector2(
        contentRect.position.x,
        validationRect.position.y - Gap - FieldHeight);
    filenameLabelRect.size = Vector2(FilenameLabelWidth, FieldHeight);
    filenameLabel->Arrange(filenameLabelRect);

    Rect filenameFieldRect;
    filenameFieldRect.position = Vector2(
        filenameLabelRect.position.x + filenameLabelRect.size.x,
        filenameLabelRect.position.y);
    filenameFieldRect.size = Vector2(
        Math::Max(0.0f, contentRect.size.x - FilenameLabelWidth),
        FieldHeight);
    filenameField->Arrange(filenameFieldRect);

    Rect gridRect;
    gridRect.position = Vector2(
        contentRect.position.x,
        breadcrumbRect.position.y + breadcrumbRect.size.y + Gap);
    gridRect.size = Vector2(
        contentRect.size.x,
        Math::Max(0.0f, filenameLabelRect.position.y - Gap - gridRect.position.y));
    contentScroll->Arrange(gridRect);
}

VirtualPath SaveSceneDialog::GetSavePath() const
{
    VirtualPath path = currentDirectory.Join(filenameField->GetText());

    if (path.GetExtension().empty())
        path = path.ReplaceExtension(".scene");

    return path;
}

Vector2 SaveSceneDialog::OnMeasure()
{
    return Vector2(DialogWidth, DialogHeight);
}

void SaveSceneDialog::OnUpdate(float)
{
    if (!pendingDirectory)
        return;

    const VirtualPath path = *pendingDirectory;
    pendingDirectory.reset();

    NavigateTo(path);
}

bool SaveSceneDialog::CanAccept() const
{
    return fileSystem.IsDirectory(currentDirectory) && IsValidFilename();
}

void SaveSceneDialog::NavigateTo(const VirtualPath& path)
{
    if (!fileSystem.IsDirectory(path))
        return;

    currentDirectory = path;
    breadcrumbBar->SetPath(currentDirectory);
    fileGrid->SetDirectory(fileSystem, currentDirectory);
}

bool SaveSceneDialog::IsValidFilename() const
{
    const String& filename = filenameField->GetText();

    if (filename.empty() ||
        filename.find_first_not_of(" \t\r\n") == String::npos ||
        filename.find_first_of("/\\:") != String::npos)
    {
        return false;
    }

    const String extension = VirtualPath(filename).GetExtension();
    return extension.empty() || extension == ".scene";
}

void SaveSceneDialog::SetValidationMessage(const char* message)
{
    validationLabel->SetText(message);
}

void SaveSceneDialog::OnFilenameChanged()
{
    SetValidationMessage("");
}

} // namespace URay
