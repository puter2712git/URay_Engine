#include "Editor/Widget/Panel/Filesystem/FileSystemPanel.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"

#include "Core/File/VirtualFilesystem.h"

#include <imgui/imgui.h>

namespace URay
{

FileSystemPanel::FileSystemPanel() : Panel("File System")
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    fileSystem = &assetSystem.GetFileSystem();

    SetBackgroundColor(Color(0.1f, 0.1f, 0.1f, 1.0f));
    SetOutlineWidth(2.0f);
    SetOutlineColor(Color::Black);

    layout = AddChild<HorizontalLayout>();

    // auto sourceAssetDirectory = std::make_unique<DirectoryEntryWidget>("Project://Asset/Source");
    // layout->AddChild(std::move(sourceAssetDirectory));
}

void FileSystemPanel::Arrange(const Rect& rect)
{
    Panel::Arrange(rect);

    Rect layoutRect = {};
    layoutRect.position = rect.position;
    layoutRect.size = Vector2(rect.size.x, 100.0f);

    layout->Arrange(layoutRect);
}

void FileSystemPanel::OnPaint(Render::UIDrawContext& context)
{
    Panel::OnPaint(context);
}

} // namespace URay
