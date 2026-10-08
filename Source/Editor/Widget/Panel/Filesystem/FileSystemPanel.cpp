#include "Editor/Widget/Panel/Filesystem/FileSystemPanel.h"

#include "Editor/Widget/Layout/VerticalLayout.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Engine.h"

#include "Core/File/VirtualFilesystem.h"

namespace URay
{

FileSystemPanel::FileSystemPanel()
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    fileSystem = &assetSystem.GetFileSystem();

    SetBackgroundColor(Color(0.1f, 0.1f, 0.1f));

    auto verticalLayout = std::make_unique<VerticalLayout>();
    directoryLayout = verticalLayout.get();
    directoryLayout->SetPadding(Padding(8.0f));
    directoryLayout->SetGap(4.0f);
    directoryLayout->SetDesiredSize(Vector2(220.0f, 0.0f));
    AddChild(std::move(verticalLayout));

    for (const VirtualFileEntry& entry : fileSystem->ListDirectory("Project://Asset"))
    {
        if (!entry.isDirectory)
            continue;

        auto directoryEntry = std::make_unique<DirectoryEntryWidget>(entry.path);
        directoryEntries.push_back(directoryEntry.get());
        directoryLayout->AddChild(std::move(directoryEntry));
    }
}

void FileSystemPanel::Arrange(const Rect& rect)
{
    Panel::Arrange(rect);

    Rect layoutRect = {};
    layoutRect.position = rect.position;
    layoutRect.size = Vector2(directoryLayout->GetDesiredSize().x, rect.size.y);

    directoryLayout->Arrange(layoutRect);
}

void FileSystemPanel::OnPaint(Render::UIDrawContext& context)
{
    Panel::OnPaint(context);
}

} // namespace URay
