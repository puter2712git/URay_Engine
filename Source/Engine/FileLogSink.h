#pragma once

#include "Core/Log/LogSink.h"

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
    void Write(std::string_view msg) override;

private:
    std::ofstream stream;
};

} // namespace URay
