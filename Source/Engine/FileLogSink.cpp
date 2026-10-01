#include "FileLogSink.h"

#include <iostream>
#include <system_error>

namespace URay
{

FileLogSink::FileLogSink(const std::filesystem::path& filePath)
{
    const std::filesystem::path directory = filePath.parent_path();

    if (!directory.empty())
    {
        std::error_code error;
        std::filesystem::create_directories(directory, error);

        if (error)
        {
            std::cerr
                << "[Log] Failed to create log directory: "
                << directory.string()
                << " (" << error.message() << ")\n";
            return;
        }
    }

    stream.open(filePath, std::ios::out | std::ios::trunc);

    if (!stream.is_open())
    {
        std::cerr
            << "[Log] Failed to open log file: "
            << filePath.string()
            << "\n";
    }
}

FileLogSink::~FileLogSink() = default;

void FileLogSink::Write(std::string_view msg)
{
    if (!stream.is_open())
        return;

    stream << msg << '\n';
}

} // namespace URay
