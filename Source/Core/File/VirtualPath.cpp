#include "VirtualPath.h"

namespace URay
{

namespace
{

constexpr const char* MountSeparator = "://";

size_t GetFilenameStart(const std::string& value)
{
    const size_t slashPos = value.find_last_of('/');
    return slashPos == std::string::npos ? 0 : slashPos + 1;
}

size_t GetExtensionStart(const std::string& value)
{
    const size_t filenameStart = GetFilenameStart(value);
    const size_t dotPos = value.find_last_of('.');

    if (dotPos == std::string::npos || dotPos < filenameStart || dotPos == filenameStart)
        return std::string::npos;

    return dotPos;
}

} // namespace

VirtualPath::VirtualPath(const char* path)
    : path(path)
{
}

VirtualPath::VirtualPath(const std::string& path)
    : path(path)
{
}

VirtualPath VirtualPath::GetDirectory() const
{
    const size_t slashPos = path.find_last_of('/');
    if (slashPos == std::string::npos)
        return VirtualPath("");

    const size_t separatorPos = path.find(MountSeparator);
    if (separatorPos != std::string::npos && slashPos == separatorPos + 2)
        return VirtualPath(path.substr(0, slashPos + 1));

    return VirtualPath(path.substr(0, slashPos));
}

std::string VirtualPath::GetFilename() const
{
    return path.substr(GetFilenameStart(path));
}

std::string VirtualPath::GetStem() const
{
    const size_t extensionPos = GetExtensionStart(path);
    if (extensionPos == std::string::npos)
        return GetFilename();

    return path.substr(GetFilenameStart(path), extensionPos - GetFilenameStart(path));
}

std::string VirtualPath::GetExtension() const
{
    const size_t extensionPos = GetExtensionStart(path);
    if (extensionPos == std::string::npos)
        return "";

    return path.substr(extensionPos);
}

VirtualPath VirtualPath::RemoveExtension() const
{
    const size_t extensionPos = GetExtensionStart(path);
    if (extensionPos == std::string::npos)
        return *this;

    return VirtualPath(path.substr(0, extensionPos));
}

VirtualPath VirtualPath::ReplaceExtension(const std::string& extension) const
{
    const size_t extensionPos = GetExtensionStart(path);
    std::string result = extensionPos == std::string::npos ? path : path.substr(0, extensionPos);

    if (extension.empty())
        return VirtualPath(result);

    if (extension.front() != '.')
        result += '.';

    result += extension;
    return VirtualPath(result);
}

VirtualPath VirtualPath::Join(const std::string& child) const
{
    if (path.empty())
        return VirtualPath(child);

    if (child.empty())
        return *this;

    std::string result = path;
    const size_t separatorPos = result.find(MountSeparator);
    const size_t minimumLength = separatorPos == std::string::npos && result.front() == '/'
        ? 1
        : (separatorPos == std::string::npos ? 0 : separatorPos + 3);

    while (result.size() > minimumLength && result.back() == '/')
        result.pop_back();

    const size_t childStart = child.find_first_not_of('/');
    if (childStart == std::string::npos)
        return VirtualPath(result);

    if (result.back() != '/')
        result += '/';

    result += child.substr(childStart);
    return VirtualPath(result);
}

std::string VirtualPath::GetMountName() const
{
    const size_t separatorPos = path.find(MountSeparator);
    if (separatorPos == std::string::npos)
        return "";

    return path.substr(0, separatorPos);
}

std::string VirtualPath::GetRelativePath() const
{
    const size_t separatorPos = path.find(MountSeparator);
    if (separatorPos == std::string::npos)
        return path;

    return path.substr(separatorPos + 3);
}

} // namespace URay
