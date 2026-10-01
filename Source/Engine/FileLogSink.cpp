#include "FileLogSink.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Engine.h"

#include "Core/File/VirtualFileSystem.h"

#include <filesystem>
#include <iostream>

namespace URay
{

FileLogSink::FileLogSink(const VirtualPath& filePath)
{
    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    VirtualFileSystem& fileSystem = assetSystem.GetFileSystem();

    std::filesystem::path path = fileSystem.ResolveToPhysicalPath(filePath);

    std::filesystem::create_directories(path.parent_path());

    stream.open(path, std::ios::out | std::ios::trunc);
    if (!stream.is_open())
    {
        std::cerr << "Failed to open log filestream." << std::endl;
    }
}

FileLogSink::~FileLogSink() = default;

void FileLogSink::Write(std::string_view msg)
{
    stream << msg << '\n';
}

} // namespace URay
