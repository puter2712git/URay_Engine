#include "FileIO.h"

#include <fstream>

namespace URay
{

bool FileIO::Exists(const fs::path& path)
{
    return fs::exists(path);
}

std::string FileIO::ReadText(const fs::path& path)
{
    if (!Exists(path))
        return std::string();

    std::ifstream file(path);
    if (!file || !file.is_open())
        return std::string();

    return std::string(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>());
}

std::vector<uint8_t> FileIO::ReadBinary(const fs::path& path)
{
    if (!Exists(path))
        return std::vector<uint8_t>();

    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file | !file.is_open())
        return std::vector<uint8_t>();

    const size_t size = static_cast<size_t>(file.tellg());
    std::vector<uint8_t> buffer(size);

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    return buffer;
}

} // namespace URay
