#pragma once

#include <Core/File/VirtualPath.h>
#include <Editor/Widget/Dialog/Dialog.h>

#include <optional>

namespace URay
{

class AssetSystem;
class BreadcrumbBar;
class Button;
class FileGridView;
class Label;
class ScrollView;
class Surface;
class VirtualFileSystem;

class ImportAssetDialog final : public Dialog
{
public:
    ImportAssetDialog(AssetSystem& assetSystem, const VirtualPath& outputDirectory);

public:
    void Arrange(const Rect& rect) override;

    const VirtualPath& GetOutputDirectory() const { return outputDirectory; }

protected:
    Vector2 OnMeasure() override;
    void OnUpdate(float deltaTime) override;

private:
    void NavigateTo(const VirtualPath& path);
    void Import(const VirtualPath& sourcePath);
    void SetValidationMessage(const char* message);

private:
    AssetSystem& assetSystem;
    VirtualFileSystem& fileSystem;
    const VirtualPath outputDirectory;
    VirtualPath currentDirectory = "External://";
    std::optional<VirtualPath> pendingDirectory = std::nullopt;

    Surface* background = nullptr;
    BreadcrumbBar* breadcrumbBar = nullptr;
    ScrollView* contentScroll = nullptr;
    FileGridView* fileGrid = nullptr;
    Label* destinationLabel = nullptr;
    Label* validationLabel = nullptr;
    Button* cancelButton = nullptr;
};

} // namespace URay
