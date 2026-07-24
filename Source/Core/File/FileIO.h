#pragma once

#include <filesystem>

namespace URay
{

namespace fs = std::filesystem;

class FileIO
{
public:
    static bool Exists(const fs::path& path);

    static std::vector<uint8_t> ReadBinary(const fs::path& path);
    static std::string ReadText(const fs::path& path);
};

} // namespace URay
