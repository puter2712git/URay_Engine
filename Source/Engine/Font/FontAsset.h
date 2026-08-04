#pragma once

#include <string>

namespace URay
{

class FontAsset
{
public:
    FontAsset(const std::string& filePath);

public:
    const std::string& GetFilePath() const { return filePath; }

private:
    std::string filePath;
};

} // namespace URay
