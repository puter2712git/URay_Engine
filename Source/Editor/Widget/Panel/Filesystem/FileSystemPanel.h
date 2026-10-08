#pragma once

#include "Editor/Widget/Layout/HorizontalLayout.h"
#include "Editor/Widget/Panel/FileSystem/DirectoryEntryWidget.h"
#include "Editor/Widget/Panel/Panel.h"

#include "Core/File/VirtualPath.h"

#include <vector>

namespace URay
{

class VirtualFileSystem;

class FileSystemPanel final : public Panel
{
    URAY_TYPE(FileSystemPanel, Panel)

public:
    FileSystemPanel();

public:
    void Arrange(const Rect& rect) override;

protected:
    void OnPaint(Render::UIDrawContext& context) override;

private:
    VirtualFileSystem* fileSystem = nullptr;

    HorizontalLayout* layout = nullptr;

    std::vector<DirectoryEntryWidget*> directoryEntries;
};

} // namespace URay
