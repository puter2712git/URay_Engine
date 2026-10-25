#include "VirtualFileSystem.h"
#include <Core/Type/Types.h>


#include <fstream>
#include <iostream>
#include <system_error>

namespace URay
{

bool VirtualFileSystem::Initialize(
    const String& enginePath,
    const String& projectPath)
{
    Mount("Engine", enginePath);
    Mount("Project", projectPath);
    Mount("Asset", std::filesystem::path(projectPath) / "Asset/Source");
    Mount("External", std::filesystem::path(projectPath) / "External");

    return true;
}

void VirtualFileSystem::Finalize()
{
    mountMap.clear();
}

void VirtualFileSystem::Mount(const String& mountName, const std::filesystem::path& physicalFilePath)
{
    mountMap.insert({ mountName, physicalFilePath });
}

bool VirtualFileSystem::Exists(const VirtualPath& path) const
{
    const std::filesystem::path physicalPath = ResolveToPhysicalPath(path);

    if (physicalPath.empty())
        return false;

    std::error_code error;
    return std::filesystem::exists(physicalPath, error) && !error;
}

bool VirtualFileSystem::IsDirectory(const VirtualPath& path) const
{
    std::error_code error;
    return std::filesystem::is_directory(
        ResolveToPhysicalPath(path), error);
}

std::vector<uint8> VirtualFileSystem::ReadBinary(const VirtualPath& virtualPath) const
{
    if (!Exists(virtualPath))
        return std::vector<uint8>();

    std::filesystem::path physicalPath = ResolveToPhysicalPath(virtualPath);

    std::ifstream file(physicalPath, std::ios::ate | std::ios::binary);
    if (!file.is_open())
        return std::vector<uint8>();

    const size_t size = static_cast<size_t>(file.tellg());
    std::vector<uint8> buffer(size);

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    return buffer;
}

String VirtualFileSystem::ReadText(const VirtualPath& virtualPath) const
{
    if (!Exists(virtualPath))
        return String();

    std::filesystem::path physicalPath = ResolveToPhysicalPath(virtualPath);

    std::ifstream file(physicalPath);
    if (!file.is_open())
        return String();

    return String(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>());
}

bool VirtualFileSystem::WriteBinary(const VirtualPath& path, const std::vector<uint8>& bin) const
{
    std::filesystem::path physicalPath = ResolveToPhysicalPath(path);

    if (physicalPath.empty())
        return false;

    if (physicalPath.has_parent_path())
    {
        std::error_code error;
        std::filesystem::create_directories(physicalPath.parent_path(), error);

        if (error)
            return false;
    }

    std::ofstream file(physicalPath, std::ios::binary);
    if (!file.is_open())
        return false;

    file.write(reinterpret_cast<const char*>(bin.data()), static_cast<std::streamsize>(bin.size()));
    return file.good();
}

bool VirtualFileSystem::WriteText(const VirtualPath& virtualPath, const String& text) const
{
    std::filesystem::path physicalPath = ResolveToPhysicalPath(virtualPath);

    if (physicalPath.empty())
        return false;

    if (physicalPath.has_parent_path())
    {
        std::error_code error;
        std::filesystem::create_directories(physicalPath.parent_path(), error);

        if (error)
            return false;
    }

    std::ofstream file(physicalPath, std::ios::out);
    if (!file.is_open())
        return false;

    file << text;
    return file.good();
}

std::vector<VirtualFileEntry> VirtualFileSystem::ListDirectory(
    const VirtualPath& directory) const
{
    std::vector<VirtualFileEntry> result;

    const std::filesystem::path physicalDirectory = ResolveToPhysicalPath(directory);

    std::error_code error;
    if (physicalDirectory.empty() ||
        !std::filesystem::is_directory(physicalDirectory, error) || error)
    {
        return result;
    }

    std::filesystem::directory_iterator iterator(physicalDirectory, error);

    if (error)
        return result;

    for (const std::filesystem::directory_entry& entry : iterator)
    {
        std::error_code directoryError;

        const bool isDirectory = entry.is_directory(directoryError);

        if (directoryError)
            continue;

        VirtualFileEntry fileEntry = {};

        const std::u8string filename = entry.path().filename().u8string();
        fileEntry.path = directory.Join(String(filename.begin(), filename.end()));

        fileEntry.isDirectory = isDirectory;

        result.push_back(fileEntry);
    }

    return result;
}

std::filesystem::path VirtualFileSystem::ResolveToPhysicalPath(const VirtualPath& virtualPath) const
{
    String pathStr = virtualPath.ToString();

    size_t pos = pathStr.find("://");

    if (pos != String::npos)
    {
        String mount = pathStr.substr(0, pos);
        String relativePath = pathStr.substr(pos + 3);

        auto it = mountMap.find(mount);
        if (it == mountMap.end())
            return std::filesystem::path();

        std::filesystem::path rootPath = it->second;

        std::filesystem::path resolvedPath = rootPath / std::filesystem::u8path(relativePath);
        return resolvedPath;
    }

    return std::filesystem::path();
}

} // namespace URay
