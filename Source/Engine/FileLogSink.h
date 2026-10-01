#pragma once

#include "Core/File/VirtualPath.h"
#include "Core/Log/LogSink.h"

#include <fstream>

namespace URay
{

class FileLogSink final : public LogSink
{
public:
    FileLogSink(const VirtualPath& filePath);
    ~FileLogSink() override;

public:
    void Write(std::string_view msg) override;

private:
    std::ofstream stream;
};

} // namespace URay
