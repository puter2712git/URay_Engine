#include "VirtualFilesystem.h"

#include <fstream>
#include <iostream>

namespace URay
{

void VirtualFilesystem::Mount(const std::string& mountName, const fs::path& physicalFilePath)
{
    mountMap.insert({ mountName, physicalFilePath });
}

bool VirtualFilesystem::Exists(const VirtualPath& virtualPath) const
{
    fs::path physicalPath = ResolveToPhysicalPath(virtualPath);
    return fs::exists(physicalPath);
}

std::vector<uint8_t> VirtualFilesystem::ReadBinary(const VirtualPath& virtualPath) const
{
    if (!Exists(virtualPath))
        return std::vector<uint8_t>();

    fs::path physicalPath = ResolveToPhysicalPath(virtualPath);

    std::ifstream file(physicalPath, std::ios::ate | std::ios::binary);
    if (!file | !file.is_open())
        return std::vector<uint8_t>();

    const size_t size = static_cast<size_t>(file.tellg());
    std::vector<uint8_t> buffer(size);

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    return buffer;
}

std::string VirtualFilesystem::ReadText(const VirtualPath& virtualPath) const
{
    if (!Exists(virtualPath))
        return std::string();

    fs::path physicalPath = ResolveToPhysicalPath(virtualPath);

    std::ifstream file(physicalPath);
    if (!file || !file.is_open())
        return std::string();

    return std::string(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>());
}

bool VirtualFilesystem::WriteText(const VirtualPath& virtualPath, const std::string& text) const
{
    fs::path physicalPath = ResolveToPhysicalPath(virtualPath);

    if (physicalPath.has_parent_path())
    {
        fs::create_directories(physicalPath.parent_path());
    }

    std::ofstream file(physicalPath, std::ios::out);
    if (!file || !file.is_open())
        return false;

    file << text;
    return file.good();
}

std::vector<VirtualFileEntry> VirtualFilesystem::ListDirectory(
    const VirtualPath& directory) const
{
    std::vector<VirtualFileEntry> result;

    const fs::path physicalDirectory = ResolveToPhysicalPath(directory);

    if (physicalDirectory.empty() || !fs::is_directory(physicalDirectory))
        return result;

    std::error_code error;
    fs::directory_iterator iterator(physicalDirectory, error);

    if (error)
        return result;

    for (const fs::directory_entry& entry : iterator)
    {
        std::error_code directoryError;

        const bool isDirectory = entry.is_directory(directoryError);

        if (directoryError)
            continue;

        VirtualFileEntry fileEntry = {};
        fileEntry.path = directory.Join(entry.path().filename().string());

        fileEntry.isDirectory = isDirectory;

        result.push_back(fileEntry);
    }

    return result;
}

fs::path VirtualFilesystem::ResolveToPhysicalPath(const VirtualPath& virtualPath) const
{
    std::string pathStr = virtualPath.ToString();

    size_t pos = pathStr.find("://");

    if (pos != std::string::npos)
    {
        std::string mount = pathStr.substr(0, pos);
        std::string relativePath = pathStr.substr(pos + 3);

        auto it = mountMap.find(mount);
        if (it == mountMap.end())
            return fs::path();

        fs::path rootPath = it->second;

        fs::path resolvedPath = rootPath / fs::u8path(relativePath);
        return resolvedPath;
    }

    return fs::path();
}

} // namespace URay
