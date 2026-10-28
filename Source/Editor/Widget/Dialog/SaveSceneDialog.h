#pragma once

#include <Core/File/VirtualPath.h>
#include <Editor/Widget/Dialog/Dialog.h>

#include <optional>

namespace URay
{

class BreadcrumbBar;
class Button;
class FileGridView;
class HorizontalLayout;
class Label;
class ScrollView;
class StringField;
class Surface;
class VirtualFileSystem;
struct VirtualFileEntry;

class SaveSceneDialog final : public Dialog
{
public:
    explicit SaveSceneDialog(VirtualFileSystem& fileSystem);

public:
    void Accept() override;
    void Arrange(const Rect& rect) override;

    VirtualPath GetSavePath() const;

protected:
    Vector2 OnMeasure() override;
    void OnUpdate(float deltaTime) override;
    bool CanAccept() const override;

private:
    void NavigateTo(const VirtualPath& path);
    bool IsValidFilename() const;
    void SetValidationMessage(const char* message);
    void OnFileEntryActivated(const VirtualFileEntry* entry);
    void OnFilenameChanged();

private:
    VirtualFileSystem& fileSystem;
    VirtualPath currentDirectory = "Asset://";
    std::optional<VirtualPath> pendingDirectory = std::nullopt;

    Surface* background = nullptr;
    BreadcrumbBar* breadcrumbBar = nullptr;
    ScrollView* contentScroll = nullptr;
    FileGridView* fileGrid = nullptr;
    Label* filenameLabel = nullptr;
    StringField* filenameField = nullptr;
    Label* validationLabel = nullptr;
    HorizontalLayout* actionLayout = nullptr;
    Button* saveButton = nullptr;
    Button* cancelButton = nullptr;
};

} // namespace URay
