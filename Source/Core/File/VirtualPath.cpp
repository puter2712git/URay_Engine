#include "VirtualPath.h"
#include <Core/Type/Types.h>

namespace URay
{

namespace
{

constexpr const char* MountSeparator = "://";

size_t GetFilenameStart(const String& value)
{
    const size_t slashPos = value.find_last_of('/');
    return slashPos == String::npos ? 0 : slashPos + 1;
}

size_t GetExtensionStart(const String& value)
{
    const size_t filenameStart = GetFilenameStart(value);
    const size_t dotPos = value.find_last_of('.');

    if (dotPos == String::npos || dotPos < filenameStart || dotPos == filenameStart)
        return String::npos;

    return dotPos;
}

} // namespace

VirtualPath::VirtualPath() = default;
VirtualPath::VirtualPath(const char* path) : path(path) {}
VirtualPath::VirtualPath(const String& path) : path(path) {}

VirtualPath VirtualPath::GetDirectory() const
{
    const size_t separatorPos = path.find(MountSeparator);
    if (separatorPos != String::npos && path.size() == separatorPos + 3)
        return VirtualPath("");

    const size_t slashPos = path.find_last_of('/');
    if (slashPos == String::npos)
        return VirtualPath("");

    if (separatorPos != String::npos && slashPos == separatorPos + 2)
        return VirtualPath(path.substr(0, slashPos + 1));

    return VirtualPath(path.substr(0, slashPos));
}

String VirtualPath::GetFilename() const
{
    return path.substr(GetFilenameStart(path));
}

String VirtualPath::GetStem() const
{
    const size_t extensionPos = GetExtensionStart(path);
    if (extensionPos == String::npos)
        return GetFilename();

    return path.substr(GetFilenameStart(path), extensionPos - GetFilenameStart(path));
}

String VirtualPath::GetExtension() const
{
    const size_t extensionPos = GetExtensionStart(path);
    if (extensionPos == String::npos)
        return "";

    return path.substr(extensionPos);
}

VirtualPath VirtualPath::RemoveExtension() const
{
    const size_t extensionPos = GetExtensionStart(path);
    if (extensionPos == String::npos)
        return *this;

    return VirtualPath(path.substr(0, extensionPos));
}

VirtualPath VirtualPath::ReplaceExtension(const String& extension) const
{
    const size_t extensionPos = GetExtensionStart(path);
    String result = extensionPos == String::npos ? path : path.substr(0, extensionPos);

    if (extension.empty())
        return VirtualPath(result);

    if (extension.front() != '.')
        result += '.';

    result += extension;
    return VirtualPath(result);
}

VirtualPath VirtualPath::Join(const String& child) const
{
    if (path.empty())
        return VirtualPath(child);

    if (child.empty())
        return *this;

    String result = path;
    const size_t separatorPos = result.find(MountSeparator);
    const size_t minimumLength = separatorPos == String::npos && result.front() == '/'
                                     ? 1
                                     : (separatorPos == String::npos ? 0 : separatorPos + 3);

    while (result.size() > minimumLength && result.back() == '/')
        result.pop_back();

    const size_t childStart = child.find_first_not_of('/');
    if (childStart == String::npos)
        return VirtualPath(result);

    if (result.back() != '/')
        result += '/';

    result += child.substr(childStart);
    return VirtualPath(result);
}

String VirtualPath::GetMountName() const
{
    const size_t separatorPos = path.find(MountSeparator);
    if (separatorPos == String::npos)
        return "";

    return path.substr(0, separatorPos);
}

String VirtualPath::GetRelativePath() const
{
    const size_t separatorPos = path.find(MountSeparator);
    if (separatorPos == String::npos)
        return path;

    return path.substr(separatorPos + 3);
}

} // namespace URay
