#include "CommandLine.h"

namespace URay
{

CommandLine::CommandLine(int argc, char** argv)
{
    arguments.reserve(argc > 1 ? argc - 1 : 0);

    for (int i = 1; i < argc; ++i)
    {
        arguments.emplace_back(argv[i]);
    }
}

CommandLine::~CommandLine() = default;

bool CommandLine::Has(const std::string& option) const
{
    for (const std::string& arg : arguments)
    {
        if (arg == option)
            return true;
    }

    return false;
}

std::optional<std::string> CommandLine::Get(const std::string& option) const
{
    size_t argumentCount = arguments.size();

    for (size_t i = 0; i < argumentCount; ++i)
    {
        if (arguments[i] != option)
            continue;

        if (i + 1 >= arguments.size())
            return std::nullopt;

        return arguments[i + 1];
    }

    return std::nullopt;
}

} // namespace URay
