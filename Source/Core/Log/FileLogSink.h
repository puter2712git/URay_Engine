#pragma once

#include <Core/Log/LogSink.h>
#include <Core/Type/TypeAlias.h>

#include <filesystem>
#include <fstream>

namespace URay
{

class FileLogSink final : public LogSink
{
public:
    FileLogSink(const std::filesystem::path& filePath);
    ~FileLogSink() override;

public:
    void Write(StringView msg) override;

private:
    std::ofstream stream;
};

} // namespace URay
